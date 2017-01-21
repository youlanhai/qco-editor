#ifndef  _GL_WIDGET_H_
#define  _GL_WIDGET_H_

#undef CursorShape

#include "config.h"
#include "platform/CCGL.h"

#ifdef glClearDepthf
#undef glClearDepthf
#endif

#ifdef glDepthRangef
#undef glDepthRangef
#endif

#include <QOpenGLWidget>
#include <QVector2D>

#include <math/CCMath.h>

class QTimer;
class QElapsedTimer;

class EDITOR_DLL GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int width, int height) override;

    const QVector2D& getMousePosition() const { return mousePosition_; }

signals:
    void signalMouseEvent(QMouseEvent *event);
    void signalKeyEvent(QKeyEvent *event);
    void signalResize(float width, float height);
    void signalWheelEvent(QWheelEvent * event);
    void signalTick(float delta);
    void signalDropFiles(const QStringList &paths, const cocos2d::Vec2 &position);

private slots:
    void onTimerUpdate();
    void cleanupCocos();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent * event) override;
    virtual void keyPressEvent(QKeyEvent *) override;
    virtual void keyReleaseEvent(QKeyEvent *) override;
    virtual void dragEnterEvent(QDragEnterEvent * event) override;
    virtual void dropEvent(QDropEvent * event) override;

private:
    bool initializeCocos();

    /** the mouse move event is very frequent, it will block the draw event.
     *  so we cached the last mouse move event, and send it before scene drawing.
     */
    void flushMouseMoveEvent();

    QTimer*         timer_;
    bool            isCocosInitialized_;
    bool            cocosInitializeResult;
    QElapsedTimer*  elapsedTimer_;

    QMouseEvent*    mouseMoveEvent_;
    QVector2D       mousePosition_;
};

#endif // _GL_WIDGET_H_
