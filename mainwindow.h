#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void ImageClustered(int parIterationCount, int parClusterCount, QImage parImage);
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void ClusterCountChange(int parClusterCount);
    //Загрузка выбранного изображения
    void LoadImage();

    void Segmentation();

    //Выбор изображения для обработки
    void SelectImage();
    
private:
    Ui::MainWindow *ui;

    //Путь к исходному изображению
    QString _ImageSourcePath;

    //Исходное изображение
    QImage _ImageSource;
};

#endif // MAINWINDOW_H
