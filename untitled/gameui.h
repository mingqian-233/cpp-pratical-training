// GameUI.h
#ifndef GAMEUI_H
#define GAMEUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVector>
#include "Cabinet.h"
#include "MedicineList.h"

class GameUI : public QWidget
{
    Q_OBJECT

public:
    explicit GameUI(QWidget *parent = nullptr);
    ~GameUI();

    void setGameData(Cabinet* cabinet, MedicineList* medicineList);
    void updateUI();
    void reset();

signals:
    void drawerClicked(int row, int col);
    void medicineSideClicked(const QString& medicineName, const QString& side);

private slots:
    void onDrawerStateChanged(int row, int col, bool isOpen);
    void onDrawerButtonClicked();
    void onMedicineSideButtonClicked();

private:
    void setupUI();
    QWidget* createMedicineListArea();
    QWidget* createCabinetArea();
    void updateMedicineListDisplay();
    void updateCabinetDisplay();

    Cabinet* m_cabinet;
    MedicineList* m_medicineList;
    QVBoxLayout* m_mainLayout;
    QVBoxLayout* m_medicineListLayout;
    QGridLayout* m_cabinetLayout;
    QWidget* m_medicineListArea;
    QVector<QVector<QPushButton*>> m_drawerButtons;
};

#endif // GAMEUI_H
