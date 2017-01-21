
#include <QApplication>
#include <QTranslator>

#include "app_delegate.h"

#include "editor/mainwindow.h"
#include "editor/launchwindow.h"
#include "editor/project_mgr.h"
#include "editor/tools/log_event_dispatcher.h"
#include "editor/tools/file_tools.h"
#include "editor/tools/log_tool.h"
#include "editor/resource.h"

#include <QDir>
#include <QStyleFactory>
#include <QFile>


int main(int argc, char **argv)
{
    //QApplication::setStyle(QStyleFactory::create("plastique"));

    // create the application instance
    QApplication app(argc, argv);

#ifdef Q_OS_MAC
    app.setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

#if QT_VERSION >= 0x050100
    // Enable support for highres images (added in Qt 5.1, but off by default)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    Editor::LogEventDispatcher::initInstance();
    Editor::ProjectManager::initInstance();
    
    std::string modulePath = Editor::getAppModulePath();
    LOG_INFO("Module path: %s", modulePath.c_str());

    std::string logFilePath = Editor::joinPath(modulePath, "editor.log");
    Editor::LogEventDispatcher::instance()->setLogFile(logFilePath);

    std::string editorResourcePath = Editor::ProjectManager::instance()->getEditorResourcePath().toStdString();
    LOG_INFO("Editor Resource Path: %s", editorResourcePath.c_str());

    QTranslator qtTranslator;
    std::string languageFile = Editor::joinPath(editorResourcePath, Editor::Res::LanguageZH);
    if(qtTranslator.load(languageFile.c_str()))
    {
        LOG_DEBUG("Succed load luanguage file '%s'", languageFile.c_str());
        app.installTranslator(&qtTranslator);
    }
    else
    {
        LOG_ERROR("Failed to load language file '%s'.", languageFile.c_str());
    }

    std::string styleSheetPath = Editor::joinPath(editorResourcePath, Editor::Res::StyleSheetFile);
    QFile styleSheetFile(QString::fromStdString(styleSheetPath));
    if(styleSheetFile.open(QFile::ReadOnly | QFile::Text))
    {
        QByteArray data = styleSheetFile.readAll();
        styleSheetFile.close();
        app.setStyleSheet(QString::fromUtf8(data));
        LOG_INFO("Set style sheet: %s", styleSheetPath.c_str());
    }

    int ret = 0;
    int retValue;
    {
        app.processEvents();

        LaunchWindow launcher;
        retValue = launcher.exec();
    }

    if (retValue == QDialog::Accepted)
    {
        AppDelegate appDelegate;

        MainWindow w;
        w.show();

        ret = app.exec();
    }

    Editor::ProjectManager::finiInstance();
    Editor::LogEventDispatcher::finiInstance();
    return ret;
}
