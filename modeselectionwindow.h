#ifndef MODESELECTIONWINDOW_H
#define MODESELECTIONWINDOW_H

#include <QMainWindow>


namespace Ui {
class ModeSelectionWindow;
}

class ModeSelectionWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ModeSelectionWindow(QWidget *parent = nullptr);
    ~ModeSelectionWindow();

private slots:
    void on_buttonMeasures_clicked();
    void on_buttonHistory_clicked();
    void on_buttonSettings_clicked();

    void on_buttonAnalyse_clicked();

private:
    Ui::ModeSelectionWindow *ui;
};

#endif // MODESELECTIONWINDOW_H
