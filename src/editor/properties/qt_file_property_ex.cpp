#include "qt_file_property_ex.h"
#include <qtattributename.h>
#include "../framework.h"
#include "../resource_mgr.h"
#include "../resource_mime.h"
#include "../tools/log_tool.h"

#include <QDir>
#include <QLineEdit>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

namespace Editor
{

    QtFilePropertyEx::QtFilePropertyEx(Type type, QtPropertyFactory *factory)
        : QtProperty(type, factory)
    {
    }

    QString QtFilePropertyEx::getValueString() const
    {
        QString uuid = value_.toString();
        QString path = Framework::instance()->resource_->uuid2path(uuid);
        return path;
    }

    QtFileEditorEx::QtFileEditorEx(QtProperty *property)
        : QtFileEditor(property)
    {
        resourceMgr_ = Framework::instance()->resource_;
        connect(resourceMgr_, SIGNAL(signalRenameFile(QString, QString)), this, SLOT(slotRenameFile(QString, QString)));

        uuid_ = property->getValue().toString();
        value_ = resourceMgr_->uuid2path(uuid_);
    }

    void QtFileEditorEx::onPropertyValueChange(QtProperty *property)
    {
        QString uuid = property->getValue().toString();
        if(uuid != uuid_)
        {
            uuid_ = uuid;
            QString path = resourceMgr_->uuid2path(uuid_);
            setValue(path);
        }
    }

    void QtFileEditorEx::slotSetAttribute(QtProperty *property, const QString &name)
    {
        if(name == QtAttributeName::FileRelativePath)
        {
            QVariant value = property->getAttribute(name);
            if(value.type() == QVariant::String)
            {
                relativePath_ = resourceMgr_->toRealPath(value.toString());
            }
        }
        else
        {
            QtFileEditor::slotSetAttribute(property, name);
        }
    }

    void QtFileEditorEx::slotEditingFinished()
    {
        QString path = input_->text();
        if(path != value_)
        {
            QString uuid;
            if(!path.isEmpty())
            {
                uuid = resourceMgr_->path2uuid(path);
                if(uuid.isEmpty())
                {
                    LOG_ERROR("The file '%s' was not found in resource manager.", path.toUtf8().data());
                    return;
                }
            }
            uuid_ = uuid;
            property_->setValue(uuid_);
        }
    }

    void QtFileEditorEx::onFileSeleted(const QString &fullPath)
    {
        QString path = fullPath;
        if(!relativePath_.isEmpty())
        {
            path = QDir(relativePath_).relativeFilePath(fullPath);
        }

        QString uuid = resourceMgr_->path2uuid(path);
        if(uuid.isEmpty())
        {
            LOG_ERROR("The file '%s' was not found in resource manager.", path.toUtf8().data());
            return;
        }

        setValue(path);
        if(uuid_ != uuid)
        {
            uuid_ = uuid;
            property_->setValue(uuid_);
        }
    }

    void QtFileEditorEx::slotRenameFile(const QString &oldName, const QString &newName)
    {
        if(value_ == oldName)
        {
            setValue(newName);
        }
    }


    bool QtFileEditorEx::eventFilter(QObject *obj, QEvent *event)
    {
        if(obj == input_)
        {
            if(event->type() == QEvent::DragEnter)
            {
                QDragEnterEvent *dragEvent = (QDragEnterEvent*)event;
                const QMimeData *data = dragEvent->mimeData();
                if(data->hasFormat(MIME_STRING_LIST))
                {
                    dragEvent->acceptProposedAction();
                    dragEvent->setDropAction(Qt::CopyAction);
                }
                else
                {
                    dragEvent->ignore();
                }
                return true;
            }
            else if(event->type() == QEvent::Drop)
            {
                QDropEvent *dropEvent = (QDropEvent*)event;
                const QMimeData *data = dropEvent->mimeData();
                if(data->hasFormat(MIME_STRING_LIST))
                {
                    dropEvent->accept();
                    dropEvent->setDropAction(Qt::CopyAction);

                    QStringList paths;
                    parseMimeStringList(paths, data->data(MIME_STRING_LIST));
                    if(!paths.empty())
                    {
                        onFileSeleted(paths[0]);
                    }
                }
                else
                {
                    dropEvent->ignore();
                }
                return true;
            }
        }

        return QtPropertyEditor::eventFilter(obj, event);
    }
}
