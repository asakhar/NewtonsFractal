#ifndef FRACTALWIDGET_H
#define FRACTALWIDGET_H

#include <QPainter>
#include <QWidget>
#include <QResizeEvent>
#include <complex>
#include <vector>
#include <QTimer>

class FractalWidget : public QWidget
{
  Q_OBJECT
public:
  explicit FractalWidget(QWidget *parent = nullptr);
  void paintEvent(QPaintEvent* /*event*/) override;
  void resizeEvent(QResizeEvent* event) override;
  void showEvent(QShowEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void resized();

private:
  using complex = std::complex<double>;
  using dot = std::pair<complex, complex>;
  std::vector<dot> dots;
  double zoom = 1000;
  std::vector<double> coeffs;
  QTimer repaint_timer;

signals:

};

#endif // FRACTALWIDGET_H
