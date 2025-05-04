#ifndef MEDICINELIST_H
#define MEDICINELIST_H

#include <map>
#include <utility>
#include <QString>
#include "Cabinet.h"

class MedicineList {
public:
    // 构造函数
    MedicineList();

            // 根据当前药柜状态生成药材清单
    void generateFromCabinetState(const Cabinet& cabinet);

            // 更新现有数量
    void updateCurrentCount(const Cabinet& cabinet);

            // 获取所有药材信息 (返回药材名, <现有数量, 目标数量>)
    std::map<QString, std::pair<int, int>> getMedicines() const;

            // 获取指定药材的现有数量和目标数量
    std::pair<int, int> getMedicineCount(const QString& name) const;

            // 检查当前药柜状态是否满足药材清单要求
    bool isSatisfied(const Cabinet& cabinet) const;

            // 清空药材清单
    void clear();

private:
    // 药材目标清单: <药材名, <现有数量, 目标数量>>
    std::map<QString, std::pair<int, int>> medicineList;
};

#endif // MEDICINELIST_H
