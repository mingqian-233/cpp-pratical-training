#include "Medicine.h"
#include <iostream>
#include <fstream>
#include <random>
#include <QFile>
#include <QTextStream>

// Medicine类构造函数
Medicine::Medicine(const QString& name) : name(name) {}

// 获取药材名称
QString Medicine::getName() const {
    return name;
}

// 获取MedicineManager单例实例
MedicineManager& MedicineManager::getInstance() {
    static MedicineManager instance;
    return instance;
}

// 初始化药材数据
bool MedicineManager::initialize() {
    return loadMedicinesFromFile(MEDICINE_FILE_PATH);
}

// 从文件加载药材
bool MedicineManager::loadMedicinesFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "无法打开文件" << std::endl;
        return false;
    }

    QTextStream in(&file);
    // 对于新版Qt，使用以下方式设置编码
    in.setEncoding(QStringConverter::Utf8);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            medicines.emplace_back(line);
        }
    }

    file.close();
    return true;
}

// 获取药材列表
const std::vector<Medicine>& MedicineManager::getMedicines() const {
    return medicines;
}

// 随机获取一个药材
const Medicine& MedicineManager::getRandomMedicine() const {
    if (medicines.empty()) {
        throw std::runtime_error("药材列表为空");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, medicines.size() - 1);

    return medicines[distrib(gen)];
}

// 随机获取n个不重复的药材
std::vector<Medicine> MedicineManager::getRandomMedicines(size_t n) const {
    if (medicines.empty()) {
        throw std::runtime_error("药材列表为空");
    }

    if (n > medicines.size()) {
        throw std::runtime_error("请求的药材数量超过了可用药材总数");
    }

    // 创建索引数组
    std::vector<size_t> indices(medicines.size());
    for (size_t i = 0; i < medicines.size(); ++i) {
        indices[i] = i;
    }

    // 随机打乱索引
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    // 选择前n个索引对应的药材
    std::vector<Medicine> result;
    result.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        result.push_back(medicines[indices[i]]);
    }

    return result;
}
