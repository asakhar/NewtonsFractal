#include "fractalwidget.h"


FractalWidget::FractalWidget(QWidget *parent) : QWidget(parent), coeffs{1., 0, 0, 1., 0, 1.}
{
//  connect(&repaint_timer, SIGNAL(timeout()), this, SLOT(repaint()));
//  repaint_timer.setInterval(1000/6);
//  repaint_timer.start();
}

void FractalWidget::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  for(auto & point : dots) {
    complex val;
    complex der;
    complex pow{1, 0};
    complex prev;
    for (double ci = coeffs.size()-1; ci >= 0; ci--) {
      val += pow*coeffs[ci];
      der += prev*(coeffs[ci]*ci);
      prev = pow;
      pow *= point.first;
    }
    point.first -= val/der;
  }
  for(auto const& point: dots) {
      painter.setPen(QColor::fromHsvF(std::fmod(std::abs(point.first.imag()+1.), 1.), 1, 1));
      painter.drawPoint(point.second.real(), point.second.imag());
  }
  painter.end();
}

void FractalWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  resized();
}

void FractalWidget::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  resized();
}

void FractalWidget::wheelEvent(QWheelEvent *event) {
  QWidget::wheelEvent(event);
  zoom += event->angleDelta().x();
  resized();
}

void FractalWidget::resized() {
  dots.resize(width()*height());
  for(double i = 0; i < width(); ++i) {
    for(double j = 0; j < height(); ++j) {
        complex point{i, j};
        dots.at(i*width()+j) = dot{point/zoom, point};
    }
  }
}
