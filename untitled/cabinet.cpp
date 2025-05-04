#include "Cabinet.h"
#include "Medicine.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <QTimer>
#include <QDebug>
// 构造函数
Cabinet::Cabinet(QObject* parent) : QObject(parent), rows(0), columns(0) {
    std::random_device rd;
    rng.seed(rd());
}

// 初始化药柜
// 初始化药柜，添加药材种类数参数
void Cabinet::initialize(int rows, int columns, int medicineTypeCount) {

    this->rows = rows;
    this->columns = columns;
    drawers.clear();
    medicineToDrawers.clear();

            // 获取药材管理器实例
    MedicineManager& medicineManager = MedicineManager::getInstance();

            // 计算需要的抽屉总数
    int totalDrawers = rows * columns;

    // 确保请求的药材种类数量合理
    const auto& allMedicines = medicineManager.getMedicines();
    int availableMedicines = allMedicines.size();

    // 如果请求的药材种类数超过可用药材，则使用所有可用药材
    medicineTypeCount = std::min(medicineTypeCount, availableMedicines);
    // 确保药材种类数至少为2（每个抽屉需要两种不同药材）
    medicineTypeCount = std::max(medicineTypeCount, 2);

    try {
        // 首先选择指定数量的药材种类
        std::vector<Medicine> selectedMedicineTypes = medicineManager.getRandomMedicines(medicineTypeCount);

        // 创建抽屉并分配药材
        for (int i = 0; i < totalDrawers; ++i) {
            // 从选定的药材类型中随机选择两种不同的药材
            int firstIndex = std::uniform_int_distribution<>(0, medicineTypeCount-1)(rng);
            int secondIndex;
            do {
                secondIndex = std::uniform_int_distribution<>(0, medicineTypeCount-1)(rng);
            } while (secondIndex == firstIndex);

            Medicine firstMed = selectedMedicineTypes[firstIndex];
            Medicine secondMed = selectedMedicineTypes[secondIndex];

            drawers.emplace_back(firstMed, secondMed, false); // 默认抽屉关闭
        }
    } catch (const std::exception& e) {
        // 处理异常，例如药材列表为空
        std::cerr << "初始化药柜失败: " << e.what() << std::endl;
        return;
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
void Cabinet::onMedicineClicked(const QString& medicineName) {
    // 查找包含该药材的所有抽屉
    auto it = medicineToDrawers.find(medicineName);
    if (it == medicineToDrawers.end()) {
        return;
    }

    const auto& drawerIndices = it->second;

    // 先收集所有需要改变状态的抽屉
    QVector<QPair<int, int>> drawersToChange;
    for (int index : drawerIndices) {
        if (index >= 0 && index < static_cast<int>(drawers.size())) {
            int row = index / columns;
            int col = index % columns;
            drawersToChange.append(qMakePair(row, col));
        }
    }

            // 批量处理状态改变
    for (const auto& pos : drawersToChange) {
        int index = pos.first * columns + pos.second;
        drawers[index].toggleState();
        emit drawerStateChanged(pos.first, pos.second, drawers[index].isOpen());
    }

            // 所有状态都已改变，发出完成信号
    emit allStateChangesCompleted();
}

// 获取药材清单
std::map<QString, std::pair<int,int>> Cabinet::getMedicineList() const {
    std::map<QString, std::pair<int,int>> result;
    for (const auto& drawer : drawers) {
        bool r=drawer.isOpen();
        const QString& firstName = drawer.getFirstMedicine().getName();
        result[firstName].first+=r;
        result[firstName].second+=r;
        const QString& secondName = drawer.getSecondMedicine().getName();
        result[secondName].first+=r;
        result[secondName].second+=r;
    }

    return result;
}

// 获取当前开启的抽屉中的药材及数量
std::map<QString, int> Cabinet::getCurrentMedicines() const {
    std::map<QString,int> result;
    for (const auto& drawer : drawers) {
        bool r=drawer.isOpen();
        if(r){
            const QString& firstName = drawer.getFirstMedicine().getName();
            result[firstName]++;
            const QString& secondName = drawer.getSecondMedicine().getName();
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
        const QString& firstName = drawer.getFirstMedicine().getName();
        medicineToDrawers[firstName].push_back(static_cast<int>(i));

        // 添加第二个药材的映射
        const QString& secondName = drawer.getSecondMedicine().getName();
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
