#ifndef QTFILEEDITOREX_H
#define QTFILEEDITOREX_H


#include "../config.h"
#include <qtproperty.h>
#include <qtpropertyeditor.h>

namespace Editor
{
    class ResourceMgr;

    class EDITOR_DLL QtFilePropertyEx : public QtProperty
    {
        Q_OBJECT
    public:
        QtFilePropertyEx(Type type, QtPropertyFactory *factory);
        virtual QString getValueString() const;
    };

    class EDITOR_DLL QtFileEditorEx : public QtFileEditor
    {
        Q_OBJECT
    public:
        explicit QtFileEditorEx(QtProperty *property);

    public slots:
        virtual void slotEditingFinished() override;
        virtual void onPropertyValueChange(QtProperty *property) override;
        virtual void slotSetAttribute(QtProperty *property, const QString &name) override;
        
        void slotRenameFile(const QString &oldName, const QString &newName);

    protected:
        virtual void onFileSeleted(const QString &fullPath) override;
        virtual bool eventFilter(QObject *obj, QEvent *event) override;

        QString         uuid_;
        ResourceMgr*    resourceMgr_;
    };

}

#endif // QTFILEEDITOREX_H
