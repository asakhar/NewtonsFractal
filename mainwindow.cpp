#include "mainwindow.h"
#include "./ui_mainwindow.h"

int constexpr SLIDER_RANGE = 1000;

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , fw{new FractalGLWidget{this}}
{
  ui->setupUi(this);
  ui->horizontalLayout->addWidget(fw);
  fw->setFixedSize(800, 800);
  connect(ui->stepsSlider, SIGNAL(sliderMoved(int)), fw, SLOT(changedSteps(int)));
  connect(ui->stepsSlider, SIGNAL(sliderMoved(int)), this, SLOT(changedSteps(int)));
  connect(ui->seedSlider, SIGNAL(sliderMoved(int)), fw, SLOT(changedSeed(int)));
  connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addPolyOrder()));
  connect(ui->btnRm, SIGNAL(clicked()), this, SLOT(rmPolyOrder()));
  connect(ui->btnAdd, SIGNAL(clicked()), fw, SLOT(addPolyOrder()));
  connect(ui->btnRm, SIGNAL(clicked()), fw, SLOT(rmPolyOrder()));
  for(int i = 0; i < polyOrder; ++i) {
    newSlider(i);
  }
}



MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::changedSteps(int pos) {
  ui->label->setText("Steps: "+ QString::number(pos));
}

void MainWindow::changedPolyOrder(int pos) {
  fw->changedPoly(polySliders[sender()], (float)pos/(float)SLIDER_RANGE);
}

void MainWindow::addPolyOrder() {
  polyOrder++;
  newSlider(polySliders.size());
}

void MainWindow::rmPolyOrder() {
  polyOrder--;
  auto iter = polySB.find(polyOrder);
  polySliders.erase(polySliders.find(iter.value()[0]));
  for(auto const elem: iter.value()) {
    delete elem;
  }
  polySB.erase(iter);
}

void MainWindow::newSlider(int i) {
  auto hLayout = new QHBoxLayout();
  auto degreeSlider = new QSlider(ui->centralwidget);
  degreeSlider->setOrientation(Qt::Horizontal);
  degreeSlider->setRange(-SLIDER_RANGE, SLIDER_RANGE);
  degreeSlider->setValue(0);
  auto text = new QLabel(this);
  text->setText(QString::number(i));
  hLayout->addWidget(text);
  hLayout->addWidget(degreeSlider);
  ui->verticalLayout->addLayout(hLayout);
  connect(degreeSlider, SIGNAL(sliderMoved(int)), this, SLOT(changedPolyOrder(int)));
  polySliders.insert(degreeSlider, i);
  polySB.insert(i, {degreeSlider, hLayout, text});
}

