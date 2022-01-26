#include "fractalglwidget.h"

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

#undef qDebug
#define qDebug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).noDebug

FractalGLWidget::FractalGLWidget(QWidget *parent) : QOpenGLWidget{parent}
{
  logger = new QOpenGLDebugLogger(this);
  connect(logger, &QOpenGLDebugLogger::messageLogged, this, &FractalGLWidget::handleLoggedMessage);
  coeffs.reserve(15);
}

void FractalGLWidget::changedPoly(int index, float pos) {
  coeffs[index] = pos;
  repaint();
}

void FractalGLWidget::changedSteps(int position) {
  numberOfSteps = position;
  repaint();
}

void FractalGLWidget::changedSeed(int pos) {
  seed = (float)pos/10000.;
  repaint();
}

void FractalGLWidget::addPolyOrder() {
  coeffs.insert(coeffs.begin(), 0.);
  repaint();
}

void FractalGLWidget::rmPolyOrder() {
  coeffs.erase(coeffs.begin());
  repaint();
}

void FractalGLWidget::initializeGL()
{
  initializeOpenGLFunctions();
  QFile shaderFile{"fractal.vert"};
  if(!shaderFile.exists()) {
    shaderFile.copy(":/fractal.vert", "fractal.vert");
  }
  shaderFile.setFileName("fractal.frag");
  if(!shaderFile.exists()) {
    shaderFile.copy(":/fractal.frag", "fractal.frag");
  }
  auto fractalVertId = initShader("fractal.vert", GL_VERTEX_SHADER);
  auto fractalFragId = initShader("fractal.frag", GL_FRAGMENT_SHADER);

  program = glCreateProgram();
  glAttachShader(program, fractalVertId);
  glAttachShader(program, fractalFragId);
  glLinkProgram(program);

  auto linkOk = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &linkOk);
  if(linkOk == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetProgramInfoLog(program, maxLength, &maxLength, errorLog.data());

    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteProgram(program); // Don't leak the program.
    glDeleteShader(fractalFragId); // Don't leak the shader.
    glDeleteShader(fractalVertId); // Don't leak the shader.
    QMessageBox(QMessageBox::Icon::Critical, "GL program linking error", errorLog.data()).exec();
    return initializeGL();
  }

  ctx = QOpenGLContext::currentContext();
  qDebug() << ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug"));
  logger->initialize();
  logger->startLogging();

  attributeCoord2d = glGetAttribLocation(program, "coord2d");
  attributeColor = glGetAttribLocation(program, "color");
  uniformCoeffs = glGetUniformLocation(program, "coeffs");
  uniformResolution = glGetUniformLocation(program, "resolution");
  unifromZoom = glGetUniformLocation(program, "zoom");
  uniformOffset = glGetUniformLocation(program, "offset");
  uniformSteps = glGetUniformLocation(program, "steps");
  uniformSeed = glGetUniformLocation(program, "seed");
  uniformLenCoeffs = glGetUniformLocation(program, "lenCoeffs");

  glClearColor(0.8, 1.0, 0.6, 1.0);
}

GLuint FractalGLWidget::initShader(const char *path, GLint type)
{
  auto shaderId = glCreateShader(type);
  QFile shaderFile{path};
  shaderFile.open(QFile::ReadOnly);
  auto shaderStd = shaderFile.readAll().toStdString();
  auto shader = shaderStd.c_str();
  glShaderSource(shaderId, 1, &shader, nullptr);
  glCompileShader(shaderId);
  auto compileOk = GL_FALSE;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileOk);
  if(compileOk == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);

    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(shaderId); // Don't leak the shader.
    QMessageBox(QMessageBox::Icon::Critical, "GL shader compilation error", errorLog.data()).exec();
    return initShader(path, type);
  }

  return shaderId;
}

void FractalGLWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(program);
  std::array<GLuint, 2> vboIds;
  glGenBuffers(vboIds.size(), &vboIds[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
  glBufferData(GL_ARRAY_BUFFER, triangleVertices.size()*sizeof(float), triangleVertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attributeCoord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(attributeCoord2d);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
  glBufferData(GL_ARRAY_BUFFER, triangleColors.size()*sizeof(float), triangleColors.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attributeColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(attributeColor);

  glUniform1fv(uniformCoeffs, coeffs.capacity(), coeffs.data());

  glUniform2f(uniformResolution, width(), height());

  glUniform2f(unifromZoom, zoom[0], zoom[1]);

  glUniform2f(uniformOffset, -offset[0], offset[1]);

  glUniform1i(uniformSteps, numberOfSteps);

  glUniform1f(uniformSeed, seed);

  glUniform1i(uniformLenCoeffs, coeffs.size());

  glDrawArrays(GL_TRIANGLE_STRIP, 0, triangleVertices.size()/2);

  glDisableVertexAttribArray(attributeCoord2d);
  glDisableVertexAttribArray(attributeColor);
}

void FractalGLWidget::resizeGL(int /*width*/, int /*height*/)
{

}

void FractalGLWidget::wheelEvent(QWheelEvent *event) {
  QOpenGLWidget::wheelEvent(event);
  zoom[1] += (GLfloat)event->angleDelta().y()/1000.*std::abs(zoom[1]);
  zoom[0] += (GLfloat)event->angleDelta().y()/1000.*std::abs(zoom[0]);
  zoom[0] += (GLfloat)event->angleDelta().x()/1000.*std::abs(zoom[0]);
  repaint();
  qDebug() << "zoom " << zoom[0] << " " << zoom[1];
}

void FractalGLWidget::mouseReleaseEvent(QMouseEvent *) {
  mouseDragging = false;
  repaint();
  qDebug() << "released";
}

void FractalGLWidget::mousePressEvent(QMouseEvent *event) {
  mouseDragging = true;
  prevMouse[0] = (GLfloat)event->x()/(GLfloat)width();
  prevMouse[1] = (GLfloat)event->y()/(GLfloat)height();
  qDebug() << "press";
}

void FractalGLWidget::mouseMoveEvent(QMouseEvent *event) {
  if(mouseDragging) {
    offset[0] += ((GLfloat)event->x()/(GLfloat)width() - prevMouse[0])*((GLfloat)width())/zoom[0]/1000.;
    offset[1] += ((GLfloat)event->y()/(GLfloat)height() - prevMouse[1])*((GLfloat)height())/zoom[1]/1000.;
    prevMouse[0] = (GLfloat)event->x()/(GLfloat)width();
    prevMouse[1] = (GLfloat)event->y()/(GLfloat)height();
    repaint();
    qDebug() << "move " << offset[0] << " " << offset[1];
  }
}

void FractalGLWidget::handleLoggedMessage(const QOpenGLDebugMessage &message) {
  qDebug() << message.message() << message.source();
}
