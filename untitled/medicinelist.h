#ifndef MEDICINELIST_H
#define MEDICINELIST_H

#include <map>
#include <string>
#include <vector>
#include "Cabinet.h"

class MedicineList {
public:
    // 构造函数
    MedicineList();

            // 根据当前药柜状态生成药材清单
    void generateFromCabinetState(const Cabinet& cabinet);

            // 获取药材清单（药材名称及其需要的数量）
    const std::map<std::string, int>& getMedicineList() const;

            // 计算当前药柜状态与目标药材清单的差异
            // 返回值: <药材名, <当前数量, 目标数量>>
    std::map<std::string, std::pair<int, int>> calculateDifference(const Cabinet& cabinet) const;

            // 检查当前药柜状态是否满足药材清单要求

    std::map<QString, std::pair<int, int>> getMedicines(const Cabinet& cabinet) const;
    // 获取所有药材及其当前数量和目标数量

    bool isSatisfied(const Cabinet& cabinet) const;

            // 清空药材清单
    void clear();

private:
    std::map<std::string, int> medicineList; // 药材名称及其需要的数量
};

#endif // MEDICINELIST_H
