#ifndef QT_COMPONENT_PROPERTY_H
#define QT_COMPONENT_PROPERTY_H

#include "../config.h"
#include <qtpropertyeditor.h>

namespace Editor
{

    class EDITOR_DLL QtComponentEditor : public QtPropertyEditor
    {
        Q_OBJECT
    public:
        explicit QtComponentEditor(QtProperty *property);
        virtual ~QtComponentEditor();

        virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory) override;

    public slots:
        virtual void onPropertyValueChange(QtProperty *property) override;
        void slotRemoveComponent();

    private:
        QWidget*            editor_;
    };

}

#endif // QT_COMPONENT_PROPERTY_H
