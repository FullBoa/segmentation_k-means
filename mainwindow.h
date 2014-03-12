#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void LoadImage();
    void SelectImage();
    
private:
    Ui::MainWindow *ui;
    QString _ImagePath;
};

#endif // MAINWINDOW_H
