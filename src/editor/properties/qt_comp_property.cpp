#include "qt_comp_property.h"
#include <qtproperty.h>
#include <QPushButton>
#include <QMessageBox>

namespace Editor
{
    QtComponentEditor::QtComponentEditor(QtProperty *property)
        : QtPropertyEditor(property)
        , editor_(nullptr)
    {

    }

    QtComponentEditor::~QtComponentEditor()
    {

    }

    QWidget* QtComponentEditor::createEditor(QWidget *parent, QtPropertyEditorFactory *factory)
    {
        if(nullptr == editor_)
        {
            QPushButton *button = new QPushButton(tr("Delete Component"), parent);
            connect(button, SIGNAL(clicked(bool)), this, SLOT(slotRemoveComponent()));
            connect(button, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
            editor_ = button;
        }
        return editor_;
    }

    void QtComponentEditor::onPropertyValueChange(QtProperty * /*property*/)
    {

    }

    void QtComponentEditor::slotRemoveComponent()
    {
        if(QMessageBox::Yes == QMessageBox::question(nullptr, tr(""), tr("Are you sure to delete?")))
        {
            property_->deleteLater();
        }
    }
}
