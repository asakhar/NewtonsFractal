#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fractalglwidget.h"
#include "fractalwidget.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void changedSteps(int pos);
  void changedPolyOrder(int pos);
  void addPolyOrder();
  void rmPolyOrder();

private:
  void newSlider(int i);
  Ui::MainWindow *ui;
  FractalGLWidget* fw;
  QHash<QObject*, int> polySliders;
  QHash<int, std::array<QObject*, 3>> polySB;
  int polyOrder = 5;
};
#endif // MAINWINDOW_H
