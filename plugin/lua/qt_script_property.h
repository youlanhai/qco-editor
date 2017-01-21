#ifndef QT_SCRIPT_PROPERTY_H
#define QT_SCRIPT_PROPERTY_H

#include <qtproperty.h>

struct lua_State;
class QFileSystemWatcher;

namespace Editor
{

    class QtScriptProperty : public QtGroupProperty
    {
        Q_OBJECT
    public:
        QtScriptProperty(Type type, QtPropertyFactory *factory);

        virtual void setChildValue(const QString &name, const QVariant &value) override;

    protected slots:
        virtual void slotChildValueChange(QtProperty *property) override;
        void slotFileChanged(const QString &path);

    protected:
        void updateScriptFile(const QString &scriptFile);
        void parseAllProperties();
        void parseClassProperty(lua_State *L);
        void parseSingleProperty(lua_State *L, QtProperty *group);

        QString         scriptFile_;
        QtPropertyList  scriptProperties_;
        QHash<QString, QVariant> properties_;

        QString         scriptFullPath_;
        QFileSystemWatcher* fileWatcher_;
    };

} // end namespace Editor

#endif // QT_SCRIPT_PROPERTY_H
