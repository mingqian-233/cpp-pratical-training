// MedicineList.cpp
#include "MedicineList.h"
#include <algorithm>
#include <random>

// 构造函数
MedicineList::MedicineList() {
}

// 根据当前药柜状态生成药材清单
void MedicineList::generateFromCabinetState(const Cabinet& cabinet) {
    // 清空当前药材清单
    medicineList.clear();
            // 获取当前打开抽屉中的所有药材
    medicineList = cabinet.getMedicineList();
}

std::map<QString, std::pair<int, int>> MedicineList::getMedicines() const {
    // Simply return the entire medicine list, which holds <current, target> pairs
    return medicineList;
}

std::pair<int, int> MedicineList::getMedicineCount(const QString& name) const {
    // Search for the medicine by its name
    auto it = medicineList.find(name);
    if (it != medicineList.end()) {
        // If found, return the current and target quantities
        return it->second;
    } else {
        // If not found, return a default pair (0, 0)
        return {0, 0};
    }
}
void MedicineList::updateCurrentCount(const Cabinet& cabinet) {
    // 获取当前药柜中的药材及其数量
    std::map<QString, int> currentMedicines = cabinet.getCurrentMedicines();

    // 遍历medicineList中的所有药材，更新其当前数量
    for (auto& [name, countPair] : medicineList) {
        // 检查药柜中是否有该药材
        if (currentMedicines.count(name) > 0) {
            // 更新当前数量，保持目标数量不变
            countPair.first = currentMedicines[name];
        } else {
            // 如果药柜中没有该药材，将当前数量设为0
            countPair.first = 0;
        }
    }
}


// 检查当前药柜状态是否满足药材清单要求
bool MedicineList::isSatisfied(const Cabinet& cabinet) const {
    for (const auto& [name, cnt] : medicineList) {
        // 如果当前没有这种药材，或者数量不匹配
        const auto [currentCount,targetCount]=cnt;
        if ( currentCount != targetCount) {
            return false;
        }
    }
    return true;
}

// 清空药材清单
void MedicineList::clear() {
    medicineList.clear();
}
