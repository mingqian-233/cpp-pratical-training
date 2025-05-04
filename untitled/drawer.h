#ifndef DRAWER_H
#define DRAWER_H

#include "Medicine.h"
#include <string>

class Drawer {
public:
    // 构造函数
    Drawer(const Medicine& firstMedicine, const Medicine& secondMedicine, bool isOpen = false);

    // 获取第一个药材
    const Medicine& getFirstMedicine() const;

    // 获取第二个药材
    const Medicine& getSecondMedicine() const;

    // 检查抽屉是否包含指定药材
    bool containsMedicine(const QString& medicineName) const;

    // 获取抽屉状态
    bool isOpen() const;

    // 切换抽屉状态（开->关，关->开）
    void toggleState();

    // 设置抽屉状态
    void setState(bool open);

private:
    Medicine firstMedicine;  // 第一个药材
    Medicine secondMedicine; // 第二个药材
    bool open;              // 抽屉状态：true为开，false为关
};

#endif // DRAWER_H
