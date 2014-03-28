#ifndef DIALOGRESULT_H
#define DIALOGRESULT_H

#include <QDialog>

namespace Ui {
class DialogResult;
}

class DialogResult : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogResult(QWidget *parent = 0);
    ~DialogResult();

    void ShowResult(int parIterationCount, int parClusterCount, QImage parImage);
    
private:
    Ui::DialogResult *ui;
};

#endif // DIALOGRESULT_H
