#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>


class PreferencesHelper;
namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

signals:
    void finished();

private:
    Ui::SettingsWindow *ui;
    PreferencesHelper *helper_;
};

#endif // SETTINGSWINDOW_H
