#include "Cabinet.h"
#include "Medicine.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <QTimer>

// 构造函数
Cabinet::Cabinet(QObject* parent) : QObject(parent), rows(0), columns(0) {
}

// 初始化药柜
void Cabinet::initialize(int rows, int columns) {
    this->rows = rows;
    this->columns = columns;
    drawers.clear();
    medicineToDrawers.clear();

    // 获取药材管理器实例
    MedicineManager& medicineManager = MedicineManager::getInstance();

    // 计算需要的药材数量（每个抽屉2种，不重复）
    int totalDrawers = rows * columns;
    int uniqueMedicinesNeeded = totalDrawers * 2;

    // 获取足够的随机药材
    std::vector<Medicine> randomMedicines;
    try {
        // 如果药材库中的药材不够，则使用所有可用药材并允许重复
        const auto& allMedicines = medicineManager.getMedicines();
        if (allMedicines.size() >= uniqueMedicinesNeeded) {
            randomMedicines = medicineManager.getRandomMedicines(uniqueMedicinesNeeded);
        } else {
            // 获取所有可用药材
            randomMedicines = allMedicines;

            // 如果药材不够，则随机选择一些重复使用
            while (randomMedicines.size() < uniqueMedicinesNeeded) {
                const Medicine& randomMed = medicineManager.getRandomMedicine();
                randomMedicines.push_back(randomMed);
            }
        }
    } catch (const std::exception& e) {
        // 处理异常，例如药材列表为空
        std::cerr << "初始化药柜失败: " << e.what() << std::endl;
        return;
    }

    // 创建抽屉并分配药材
    for (int i = 0; i < totalDrawers; ++i) {
        Medicine firstMed = randomMedicines[i * 2];
        Medicine secondMed = randomMedicines[i * 2 + 1];
        drawers.emplace_back(firstMed, secondMed, false); // 默认抽屉关闭
    }

    // 初始化药材到抽屉的映射
    initializeMedicineToDrawersMap();
}

// 获取指定位置的抽屉
Drawer* Cabinet::getDrawer(int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= columns) {
        return nullptr;
    }
    int index = row * columns + col;
    if (index < 0 || index >= static_cast<int>(drawers.size())) {
        return nullptr;
    }
    return &drawers[index];
}

// 获取所有抽屉
const std::vector<Drawer>& Cabinet::getAllDrawers() const {
    return drawers;
}

// 处理药材被点击的事件
void Cabinet::onMedicineClicked(const std::string& medicineName) {
    // 查找包含该药材的所有抽屉
    auto it = medicineToDrawers.find(medicineName);
    if (it == medicineToDrawers.end()) {
        return;
    }

    const auto& drawerIndices = it->second;

    // 切换每个包含该药材的抽屉的状态
    for (int index : drawerIndices) {
        if (index >= 0 && index < static_cast<int>(drawers.size())) {
            drawers[index].toggleState();

            // 计算行列位置
            int row = index / columns;
            int col = index % columns;

            // 发出抽屉状态改变的信号
            emit drawerStateChanged(row, col, drawers[index].isOpen());
        }
    }

    // 使用QTimer确保所有状态变化信号都已处理后再发出完成信号
    QTimer::singleShot(0, this, &Cabinet::allStateChangesCompleted);
}

// 检查当前开启的抽屉中的药材是否符合目标清单
bool Cabinet::checkMedicineList(const std::map<std::string, int>& targetList) const {
    std::map<std::string, int> currentMedicines = getCurrentMedicines();

    // 检查药材种类和数量是否一致
    if (currentMedicines.size() != targetList.size()) {
        return false;
    }

    for (const auto& pair : targetList) {
        const std::string& medicineName = pair.first;
        int targetCount = pair.second;

        auto it = currentMedicines.find(medicineName);
        if (it == currentMedicines.end() || it->second != targetCount) {
            return false;
        }
    }

    return true;
}

// 获取当前开启的抽屉中的药材及数量
std::map<std::string, int> Cabinet::getCurrentMedicines() const {
    std::map<std::string, int> result;

    for (const auto& drawer : drawers) {
        if (drawer.isOpen()) {
            // 增加第一个药材的计数
            const std::string& firstName = drawer.getFirstMedicine().getName();
            result[firstName]++;

            // 增加第二个药材的计数
            const std::string& secondName = drawer.getSecondMedicine().getName();
            result[secondName]++;
        }
    }

    return result;
}

// 获取药柜的行数
int Cabinet::getRows() const {
    return rows;
}

// 获取药柜的列数
int Cabinet::getColumns() const {
    return columns;
}

// 随机设置一些抽屉的初始状态为打开
void Cabinet::randomizeInitialState(int openDrawersCount) {
    // 确保不超过抽屉总数
    openDrawersCount = std::min(openDrawersCount, static_cast<int>(drawers.size()));

    // 创建索引数组
    std::vector<int> indices(drawers.size());
    for (size_t i = 0; i < drawers.size(); ++i) {
        indices[i] = static_cast<int>(i);
    }

    // 随机打乱索引
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    // 重置所有抽屉为关闭状态
    for (auto& drawer : drawers) {
        drawer.setState(false);
    }

    // 设置指定数量的抽屉为打开状态
    for (int i = 0; i < openDrawersCount; ++i) {
        int index = indices[i];
        drawers[index].setState(true);

        // 计算行列位置
        int row = index / columns;
        int col = index % columns;

        // 发出抽屉状态改变的信号
        emit drawerStateChanged(row, col, true);
    }

    // 发出所有状态变化完成的信号
    emit allStateChangesCompleted();
}

// 初始化药材到抽屉的映射
void Cabinet::initializeMedicineToDrawersMap() {
    medicineToDrawers.clear();

    for (size_t i = 0; i < drawers.size(); ++i) {
        const Drawer& drawer = drawers[i];

        // 添加第一个药材的映射
        const std::string& firstName = drawer.getFirstMedicine().getName();
        medicineToDrawers[firstName].push_back(static_cast<int>(i));

        // 添加第二个药材的映射
        const std::string& secondName = drawer.getSecondMedicine().getName();
        medicineToDrawers[secondName].push_back(static_cast<int>(i));
    }
}

bool Cabinet::isDrawerOpen(int row, int col) const {
    if (row >= 0 && row < rows && col >= 0 && col < columns) {
        int index = row * columns + col;
        if (index < drawers.size()) {
            return drawers[index].isOpen();
        }
    }
    return false;
}
