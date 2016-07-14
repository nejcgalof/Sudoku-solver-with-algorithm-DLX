#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QWidget *centralWidget;
    QGridLayout*  mainLayout;

signals:
    void signal1();

private slots:
    void algoritem();
    void reset();
    void hamza();
public:
    explicit MainWindow(QWidget *parent = 0);
    void GetNumbers();
    void print_sudoku_array();
    void BuildData();
    void CreateMatrix();
    void LoadPuzzle();
    void Search(int k);
    void put_results();

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPushButton *m_button;
};

#endif // MAINWINDOW_H
