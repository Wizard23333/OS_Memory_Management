#ifndef GRAPHRAM_H
#define GRAPHRAM_H

#include <QWidget>

namespace Ui {
class graphRAM;
}

class graphRAM : public QWidget
{
    Q_OBJECT

public:
    explicit graphRAM(QWidget *parent = nullptr);
    ~graphRAM();

private:
    Ui::graphRAM *ui;
};

#endif // GRAPHRAM_H
