#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Обработка кнопки "Загрузить изображение"
    connect(ui->buttonLoadImage, SIGNAL(clicked()), this, SLOT(LoadImage()));

    //Обработка кнопки "Обзор" для выбора изображения
    connect(ui->buttonSelectImage, SIGNAL(clicked()), this, SLOT(SelectImage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Загрузка изображения
void MainWindow::LoadImage()
{
    //Если путь к изображению не пустой, то
    if (ui->lineEditImagePath->text() != "")
    {
        //загружаем изображение и
        _ImageSource = QImage(_ImageSourcePath);

        //выводим его на экран
        ui->labelImageSource->setScaledContents(true);
        ui->labelImageSource->setPixmap(QPixmap().fromImage(_ImageSource));
    }
}

//Выбор изображения для обработки
void MainWindow::SelectImage()
{
    //Открываем диалог выбора файла и запоминаем путь к выбранному файлу
    _ImageSourcePath = QFileDialog::getOpenFileName(this, QString::fromUtf8("Исходное изображение"), QDir::homePath());

    //Выводим путь к файлу с помощью lineEdit
    ui->lineEditImagePath->setText(_ImageSourcePath);
}
