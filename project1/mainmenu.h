// mainmenu.h
#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include "backgroundwidget.h"

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onStoryModeClicked();
    void onChallengeModeClicked();
    void onCustomModeClicked();
    void onSettingsClicked();
    void onExitClicked();

private:
    BackgroundWidget *backgroundWidget;
    void setupUI();
};

#endif // MAINMENU_H
