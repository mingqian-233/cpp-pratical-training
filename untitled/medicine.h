#ifndef MEDICINE_H
#define MEDICINE_H

#include <vector>
#include<QString>
class Medicine {
public:
    // 构造函数
    Medicine(const QString& name);

            // 获取药材名称
    QString getName() const;

private:
    QString name; // 药材名称
};


class MedicineManager {
public:
    // 获取单例实例
    static MedicineManager& getInstance();

            // 禁止拷贝和赋值
    MedicineManager(const MedicineManager&) = delete;
    MedicineManager& operator=(const MedicineManager&) = delete;

            // 初始化药材数据
    bool initialize();

            // 获取药材列表
    const std::vector<Medicine>& getMedicines() const;

    // 随机获取一个药材
    const Medicine& getRandomMedicine() const;

    // 随机获取n个不重复的药材
    std::vector<Medicine> getRandomMedicines(size_t n) const;

private:
    // 私有构造函数
    MedicineManager() = default;

    // 从文件加载药材
    bool loadMedicinesFromFile(const QString& filename);

    std::vector<Medicine> medicines; // 药材列表
    const QString MEDICINE_FILE_PATH = ":/data/medicine_name.txt"; // 药材名称文件路径
};


#endif // MEDICINE_H
