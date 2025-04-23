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
    medicineList = cabinet.getCurrentMedicines();
}

// 获取药材清单
const std::map<std::string, int>& MedicineList::getMedicineList() const {
    return medicineList;
}

// 计算当前药柜状态与目标药材清单的差异
std::map<std::string, std::pair<int, int>> MedicineList::calculateDifference(const Cabinet& cabinet) const {
    std::map<std::string, std::pair<int, int>> difference;

    // 获取当前打开抽屉中的所有药材及其数量
    std::map<std::string, int> currentMedicines = cabinet.getCurrentMedicines();

    // 首先将所有目标药材添加到差异表中
    for (const auto& [name, targetCount] : medicineList) {
        int currentCount = currentMedicines.count(name) ? currentMedicines[name] : 0;
        difference[name] = std::make_pair(currentCount, targetCount);
    }

    // 添加当前有但目标中没有的药材
    for (const auto& [name, currentCount] : currentMedicines) {
        if (medicineList.count(name) == 0) {
            difference[name] = std::make_pair(currentCount, 0);
        }
    }

    return difference;
}

// 检查当前药柜状态是否满足药材清单要求
bool MedicineList::isSatisfied(const Cabinet& cabinet) const {
    // 获取当前打开抽屉中的所有药材及其数量
    std::map<std::string, int> currentMedicines = cabinet.getCurrentMedicines();

    // 检查每种目标药材的数量是否匹配
    for (const auto& [name, targetCount] : medicineList) {
        // 如果当前没有这种药材，或者数量不匹配
        if (currentMedicines.count(name) == 0 || currentMedicines[name] != targetCount) {
            return false;
        }
    }

    // 检查是否有多余的药材（当前有但目标中没有）
    for (const auto& [name, currentCount] : currentMedicines) {
        if (medicineList.count(name) == 0) {
            return false;
        }
    }

    return true;
}

// 清空药材清单
void MedicineList::clear() {
    medicineList.clear();
}

std::map<QString, std::pair<int, int>> MedicineList::getMedicines(const Cabinet& cabinet) const {
    std::map<QString, std::pair<int, int>> result;

    // Get current medicines from cabinet
    std::map<std::string, int> currentMedicines = cabinet.getCurrentMedicines();

    // Process all medicines in the target list
    for (const auto& [name, targetCount] : medicineList) {
        QString medicineName = QString::fromStdString(name);
        int currentCount = 0;

        // If this medicine exists in current medicines, get its count
        if (currentMedicines.count(name) > 0) {
            currentCount = currentMedicines[name];
        }

        result[medicineName] = std::make_pair(currentCount, targetCount);
    }

    return result;
}


