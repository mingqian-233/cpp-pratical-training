#include "Drawer.h"

// 构造函数
Drawer::Drawer(const Medicine& firstMedicine, const Medicine& secondMedicine, bool isOpen)
    : firstMedicine(firstMedicine), secondMedicine(secondMedicine), open(isOpen) {
}

// 获取第一个药材
const Medicine& Drawer::getFirstMedicine() const {
    return firstMedicine;
}

// 获取第二个药材
const Medicine& Drawer::getSecondMedicine() const {
    return secondMedicine;
}

// 检查抽屉是否包含指定药材
bool Drawer::containsMedicine(const std::string& medicineName) const {
    return (firstMedicine.getName() == medicineName ||
            secondMedicine.getName() == medicineName);
}

// 获取抽屉状态
bool Drawer::isOpen() const {
    return open;
}

// 切换抽屉状态
void Drawer::toggleState() {
    open = !open;
}

// 设置抽屉状态
void Drawer::setState(bool isOpen) {
    open = isOpen;
}
