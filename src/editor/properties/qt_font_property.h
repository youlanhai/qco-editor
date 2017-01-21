#ifndef QT_FONT_PROPERTY_H
#define QT_FONT_PROPERTY_H

#include <qtpropertyeditor.h>

namespace Editor
{
    class QtFontEditor : public QtEnumPairEditor
    {
        Q_OBJECT
    public:
        explicit QtFontEditor(QtProperty *property);

    public slots:
        virtual void slotSetAttribute(QtProperty * property, const QString &name) override;

    protected:
        QStringList     enumNames_;
    };
}

#endif // QT_FONT_PROPERTY_H
