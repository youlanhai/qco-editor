
#include "glwidget.h"

#include <QTimer>
#include <QElapsedTimer>
#include <QOpenGLContext>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QMessageBox>
#include <QScreen>
#include <QMimeData>

#include <base/CCDirector.h>
#include <platform/CCApplication.h>
#include <renderer/CCRenderer.h>
#include "qtglview.h"
#include "tools/log_tool.h"
#include "resource_mime.h"
#include "framework.h"

USING_NS_CC;

DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "GLWidget");

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , timer_(NULL)
    , isCocosInitialized_(false)
    , elapsedTimer_(nullptr)
    , mouseMoveEvent_(nullptr)
    , cocosInitializeResult(true)
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSamples(4);
    //format.setVersion(3, 2);
    //format.setRenderableType(QSurfaceFormat::OpenGLES);

    setFormat(format);

    LOG_DEBUG("construct.");
}

GLWidget::~GLWidget()
{
    delete timer_;
    delete elapsedTimer_;

    LOG_DEBUG("destruct.");
}

void GLWidget::initializeGL()
{
    LOG_DEBUG("initializeGL");

    connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(cleanupCocos()));

    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));

    timer_->start(1000 / 60);

    elapsedTimer_ = new QElapsedTimer();
    elapsedTimer_->start();
}

void GLWidget::paintGL()
{
    if(!isCocosInitialized_ || !cocosInitializeResult)
    {
        return;
    }

    if(mouseMoveEvent_ != nullptr)
    {
        flushMouseMoveEvent();
    }

    Director::getInstance()->mainLoop();
}

void GLWidget::onTimerUpdate()
{
    //因为GLWidget::initializeGL函数中，默认的FrameBuffer还没有创建完成
    //cocos将无法获得正确的FrameBuffer，需要延迟到渲染这里。
    if(!isCocosInitialized_ && isValid())
    {
        makeCurrent();

        isCocosInitialized_ = true;
        if(!initializeCocos())
        {
            QMessageBox::critical(nullptr, tr("Error"), tr("Initialize application failed! For more information plase see the log."));
            QCoreApplication::instance()->quit();
            return;
        }
    }

    double delta = elapsedTimer_->elapsed() * 0.001;
    emit signalTick((float)delta);
    elapsedTimer_->restart();

    this->update();
}

void GLWidget::resizeGL(int width, int height)
{
    makeCurrent();
    
    auto director = cocos2d::Director::getInstance();
    GLView* view = director->getOpenGLView();
    if (view)
    {
        float scale = context()->screen()->devicePixelRatio();
        view->setFrameSize(width * scale, height * scale);
        view->setDesignResolutionSize(width, height, ResolutionPolicy::SHOW_ALL);
//        view->setViewPortInPoints(0, 0, width, height);
    }

    emit signalResize(width, height);
}

bool GLWidget::initializeCocos()
{
    Application::getInstance()->initGLContextAttrs();

    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview)
    {
        glview = QtGLViewImpl::create(this);
        director->setOpenGLView(glview);
    }

    cocosInitializeResult = false;
    do
    {
        if(!Application::getInstance()->applicationDidFinishLaunching())
        {
            break;
        }

        if(!Editor::Framework::instance()->init())
        {
            break;
        }

        cocosInitializeResult = true;
    }while(0);

    if(!cocosInitializeResult)
    {
        LOG_ERROR("Failed to init application!!!");
    }

    return cocosInitializeResult;
}

// cleanup opengl resource here.
void GLWidget::cleanupCocos()
{
    LOG_DEBUG("GL will destroy.");

    makeCurrent();
    Director::getInstance()->end();
    Director::getInstance()->mainLoop();
    doneCurrent();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseMoveEvent(event);

    if(mouseMoveEvent_ != nullptr)
    {
        delete mouseMoveEvent_;
    }

    mouseMoveEvent_ = new QMouseEvent(*event);

    mousePosition_.setX(event->x());
    mousePosition_.setY(event->y());
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    QOpenGLWidget::mousePressEvent(event);

    mousePosition_.setX(event->x());
    mousePosition_.setY(event->y());

    makeCurrent();
    emit signalMouseEvent(event);
    doneCurrent();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseReleaseEvent(event);

    mousePosition_.setX(event->x());
    mousePosition_.setY(event->y());

    makeCurrent();
    if(mouseMoveEvent_ != nullptr)
    {
        flushMouseMoveEvent();
    }
    emit signalMouseEvent(event);
    doneCurrent();
}

void GLWidget::wheelEvent(QWheelEvent * event)
{
    QOpenGLWidget::wheelEvent(event);

    makeCurrent();
    emit signalWheelEvent(event);
    doneCurrent();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    QOpenGLWidget::keyPressEvent(event);

    makeCurrent();
    emit signalKeyEvent(event);
    doneCurrent();
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    QOpenGLWidget::keyReleaseEvent(event);

    makeCurrent();
    emit signalKeyEvent(event);
    doneCurrent();
}

void GLWidget::flushMouseMoveEvent()
{
    CCAssert(mouseMoveEvent_ != nullptr, "flushMouseMoveEvent");
    emit signalMouseEvent(mouseMoveEvent_);

    delete mouseMoveEvent_;
    mouseMoveEvent_ = nullptr;
}

void GLWidget::dragEnterEvent(QDragEnterEvent * event)
{
    const QMimeData *data = event->mimeData();
    if(data->hasFormat(Editor::MIME_STRING_LIST))
    {
        event->acceptProposedAction();
        event->setDropAction(Qt::CopyAction);
    }
    else
    {
        event->ignore();
    }
}

void GLWidget::dropEvent(QDropEvent * event)
{
    const QMimeData *data = event->mimeData();
    if(data->hasFormat(Editor::MIME_STRING_LIST))
    {
        event->accept();
        event->setDropAction(Qt::CopyAction);

        QStringList paths;
        Editor::parseMimeStringList(paths, data->data(Editor::MIME_STRING_LIST));

        cocos2d::Vec2 pt(event->posF().x(), event->posF().y());
        pt = cocos2d::Director::getInstance()->convertToUI(pt);

        EDITOR_LOCK_GL_SCOPE();
        emit signalDropFiles(paths, pt);
    }
    else
    {
        event->ignore();
    }
}
