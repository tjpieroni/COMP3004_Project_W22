#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //Check connection when session is started, true = good connection false = no connection
    bool checkConnection();
private slots:


private:
    Ui::MainWindow *ui;
    QTimer *powerButtonTimer,*upIntensityTimer,*downIntensityTimer;

    bool powerStatus,therapyInProgress,shuttingDown;

private slots:
    void startPowerTimer();
    void handlePowerButton();
    void togglePower();
    void endSession();
    void selectGroup();
    void confirmTreatment();

    void selectUpSession();
    void selectDownSession();
    void startupIntensityTimer();
    void startdownIntensityTimer();
    void increaseIntensity();
    void decreaseIntensity();
};
#endif // MAINWINDOW_H
