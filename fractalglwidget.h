#ifndef FRACTALGLWIDGET_H
#define FRACTALGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QOpenGLDebugLogger>

class FractalGLWidget : public QOpenGLWidget, public QOpenGLFunctions
{
  Q_OBJECT
public:
  FractalGLWidget(QWidget* parent = nullptr);
  ~FractalGLWidget() override = default;
public slots:
  void changedPoly(int index, float pos);
  void changedSteps(int pos);
  void changedSeed(int pos);
  void addPolyOrder();
  void rmPolyOrder();
protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void wheelEvent(QWheelEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* /*event*/) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  virtual void handleLoggedMessage(QOpenGLDebugMessage const& message);
private:
  GLuint initShader(char const* path, GLint type);
  GLint attributeCoord2d;
  GLint attributeColor;
  GLint uniformCoeffs;
  GLint uniformResolution;
  GLint uniformOffset;
  GLint unifromZoom;
  GLint uniformSteps;
  GLint uniformSeed;
  GLint uniformLenCoeffs;
  GLuint program;
  bool mouseDragging = false;
  GLuint numberOfSteps = 4;
  float seed = 1.;
  std::array<GLfloat, 2> prevMouse;
  std::array<GLfloat, 2> zoom = {1., 1.};
  std::array<GLfloat, 2> offset = {-.5, -.5};
  std::vector<GLfloat> triangleVertices{
    -1, -1,
    1, -1,
    -1, 1,
    1, 1
  };
  std::vector<GLfloat> triangleColors{
    0.0, 0.0, 1.0,
    0.0, 1.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 1.0, 1.0
  };
  std::vector<GLfloat> coeffs{
    .6, 0., 0., .5, -1., 1.
  };
  QOpenGLContext *ctx;
  QOpenGLDebugLogger *logger;
};

#endif // FRACTALGLWIDGET_H
