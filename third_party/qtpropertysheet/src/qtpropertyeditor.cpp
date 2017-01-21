#include "qtpropertyeditor.h"
#include "qtproperty.h"
#include "qtpropertybrowserutils.h"
#include "qxtcheckcombobox.h"
#include "qtattributename.h"
#include "qtpropertyeditorfactory.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QAbstractItemView>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>
#include <QEvent>
#include <QDropEvent>
#include <QMimeData>

#include <limits>

QtPropertyEditor::QtPropertyEditor(QtProperty *property)
    : property_(property)
{
    connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onPropertyValueChange(QtProperty*)));
    connect(property, SIGNAL(destroyed(QObject*)), this, SLOT(onPropertyDestory(QObject*)));
}

QtPropertyEditor::~QtPropertyEditor()
{
}

void QtPropertyEditor::onPropertyDestory(QObject * /*object*/)
{
    property_ = NULL;
    delete this;
}

void QtPropertyEditor::slotEditorDestory(QObject * /*object*/)
{
    delete this;
}

/********************************************************************/
QtIntSpinBoxEditor::QtIntSpinBoxEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(0)
{
    value_ = property_->getValue().toInt();
    connect(property, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtIntSpinBoxEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(editor_ == 0)
    {
        editor_ = new QSpinBox(parent);
        editor_->setKeyboardTracking(false);

        slotSetAttribute(property_, QtAttributeName::MinValue);
        slotSetAttribute(property_, QtAttributeName::MaxValue);
        slotSetAttribute(property_, QtAttributeName::ReadOnly);

        editor_->setValue(value_);

        connect(editor_, SIGNAL(valueChanged(int)), this, SLOT(slotEditorValueChange(int)));
    }
    return editor_;
}

void QtIntSpinBoxEditor::slotEditorValueChange(int value)
{
    if(value == value_)
    {
        return;
    }
    value_ = value;

    if(property_ != 0)
    {
        property_->setValue(value);
    }
}

void QtIntSpinBoxEditor::onPropertyValueChange(QtProperty* property)
{
    value_ = property->getValue().toInt();
    if(editor_ != 0)
    {
        editor_->blockSignals(true);
        editor_->setValue(value_);
        editor_->blockSignals(false);
    }
}

void QtIntSpinBoxEditor::slotSetAttribute(QtProperty *property, const QString &name)
{
    if(NULL == editor_)
    {
        return;
    }

    QVariant v = property->getAttribute(QtAttributeName::MaxValue);
    if(name == QtAttributeName::MinValue)
    {
        int minValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::min();
        editor_->setMinimum(minValue);
    }
    else if(name == QtAttributeName::MaxValue)
    {
        int maxValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::max();
        editor_->setMaximum(maxValue);
    }
    else if(name == QtAttributeName::ReadOnly)
    {
        editor_->setReadOnly(v.type() == QVariant::Bool && v.toBool());
    }
}

/********************************************************************/
QtDoubleSpinBoxEditor::QtDoubleSpinBoxEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(0)
{
    value_ = property_->getValue().toDouble();
    connect(property_, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtDoubleSpinBoxEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(editor_ == 0)
    {
        editor_ = new QDoubleSpinBox(parent);
        editor_->setKeyboardTracking(false);

        slotSetAttribute(property_, QtAttributeName::MinValue);
        slotSetAttribute(property_, QtAttributeName::MaxValue);
        slotSetAttribute(property_, QtAttributeName::Decimals);
        slotSetAttribute(property_, QtAttributeName::ReadOnly);

        editor_->setValue(value_);

        connect(editor_, SIGNAL(valueChanged(double)), this, SLOT(slotEditorValueChange(double)));
    }
    return editor_;
}

void QtDoubleSpinBoxEditor::slotEditorValueChange(double value)
{
    if(value == value_)
    {
        return;
    }
    value_ = value;

    if(property_ != 0)
    {
        property_->setValue(value);
    }
}

void QtDoubleSpinBoxEditor::onPropertyValueChange(QtProperty* property)
{
    value_ = property->getValue().toDouble();
    if(editor_ != 0)
    {
        editor_->blockSignals(true);
        editor_->setValue(value_);
        editor_->blockSignals(false);
    }
}

void QtDoubleSpinBoxEditor::slotSetAttribute(QtProperty *property, const QString &name)
{
    if(NULL == editor_)
    {
        return;
    }

    QVariant v = property->getAttribute(QtAttributeName::MinValue);
    if(name == QtAttributeName::MinValue)
    {
        int minValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::min();
        editor_->setMinimum(minValue);
    }
    else if(name == QtAttributeName::MaxValue)
    {
        int maxValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::max();
        editor_->setMaximum(maxValue);
    }
    else if(name == QtAttributeName::Decimals)
    {
        if(v.type() == QVariant::Int)
        {
            editor_->setDecimals(v.toInt());
        }
    }
    else if(name == QtAttributeName::ReadOnly)
    {
        editor_->setReadOnly(v.type() == QVariant::Bool && v.toBool());
    }
}

/********************************************************************/
QtStringEditor::QtStringEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property->getValue().toString();
}

QWidget* QtStringEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(editor_ == NULL)
    {
        editor_ = new QLineEdit(parent);
        editor_->setText(value_);

        slotSetAttribute(property_, QtAttributeName::ReadOnly);

        connect(editor_, SIGNAL(editingFinished()), this, SLOT(slotEditFinished()));
    }
    return editor_;
}

void QtStringEditor::onPropertyValueChange(QtProperty *property)
{
    value_ = property->getValue().toString();
    if(editor_ != NULL)
    {
        editor_->blockSignals(true);
        editor_->setText(value_);
        editor_->blockSignals(false);
    }
}

void QtStringEditor::slotEditFinished()
{
    QString text = editor_->text();
    if(value_ != text)
    {
        value_ = text;
        if(property_ != 0)
        {
            property_->setValue(value_);
        }
    }
}

void QtStringEditor::slotSetAttribute(QtProperty *property, const QString &name)
{
    if(!editor_)
    {
        return;
    }

    QVariant value = property->getAttribute(name);
    if(name == QtAttributeName::ReadOnly)
    {
        if(value.type() == QVariant::Bool)
        {
            editor_->setReadOnly(value.toBool());
        }
    }
}

/********************************************************************/
QtEnumEditor::QtEnumEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property_->getValue().toInt();
    connect(property_, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtEnumEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(editor_ == NULL)
    {
        editor_ = new QComboBox(parent);
        editor_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        editor_->setMinimumContentsLength(1);
        editor_->view()->setTextElideMode(Qt::ElideRight);

        slotSetAttribute(property_, QtAttributeName::EnumName);

        editor_->setCurrentIndex(value_);

        connect(editor_, SIGNAL(currentIndexChanged(int)), this, SLOT(slotEditorValueChange(int)));
    }
    return editor_;
}

void QtEnumEditor::onPropertyValueChange(QtProperty *property)
{
    value_ = property->getValue().toInt();
    if(editor_ != NULL)
    {
        editor_->blockSignals(true);
        editor_->setCurrentIndex(value_);
        editor_->blockSignals(false);
    }
}

void QtEnumEditor::slotEditorValueChange(int index)
{
    if(index != value_)
    {
        value_ = index;
        property_->setValue(value_);
    }
}

void QtEnumEditor::slotSetAttribute(QtProperty * property, const QString &name)
{
    if(name == QtAttributeName::EnumName)
    {
        editor_->clear();

        QStringList enumNames = property->getAttribute(QtAttributeName::EnumName).toStringList();
        editor_->addItems(enumNames);
    }
}

/********************************************************************/
QtEnumPairEditor::QtEnumPairEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    enumValues_ = property_->getAttribute(QtAttributeName::EnumValues).toList();
    index_ = enumValues_.indexOf(property_->getValue());

    connect(property_, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtEnumPairEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(editor_ == NULL)
    {
        editor_ = new QComboBox(parent);
        editor_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        editor_->setMinimumContentsLength(1);
        editor_->view()->setTextElideMode(Qt::ElideRight);

        slotSetAttribute(property_, QtAttributeName::EnumName);

        editor_->setCurrentIndex(index_);

        connect(editor_, SIGNAL(currentIndexChanged(int)), this, SLOT(slotEditorValueChange(int)));
    }
    return editor_;
}

void QtEnumPairEditor::onPropertyValueChange(QtProperty *property)
{
    index_ = enumValues_.indexOf(property->getValue());
    if(editor_ != NULL)
    {
        editor_->blockSignals(true);
        editor_->setCurrentIndex(index_);
        editor_->blockSignals(false);
    }
}

void QtEnumPairEditor::slotEditorValueChange(int index)
{
    if(index != index_)
    {
        index_ = index;
        if(index_ >= 0 && index_ < enumValues_.size())
        {
            property_->setValue(enumValues_[index_]);
        }
    }
}

void QtEnumPairEditor::slotSetAttribute(QtProperty * property, const QString &name)
{
    if(name == QtAttributeName::EnumName)
    {
        editor_->clear();

        QStringList enumNames = property->getAttribute(QtAttributeName::EnumName).toStringList();
        editor_->addItems(enumNames);
    }
    else if(name == QtAttributeName::EnumValues)
    {
        enumValues_ = property->getAttribute(QtAttributeName::EnumValues).toList();

        int index = std::max(0, enumValues_.indexOf(property->getValue()));
        if(index != index_)
        {
            if(editor_ != NULL)
            {
                editor_->setCurrentIndex(index);
            }
        }
    }
}
/********************************************************************/
QtFlagEditor::QtFlagEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property_->getValue().toInt();
    connect(property_, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtFlagEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(NULL == editor_)
    {
        editor_ = new QxtCheckComboBox(parent);
        editor_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        editor_->setMinimumContentsLength(1);
        editor_->view()->setTextElideMode(Qt::ElideRight);
        editor_->setSeparator("|");

        slotSetAttribute(property_, QtAttributeName::FlagName);
        setValueToEditor(value_);

        connect(editor_, SIGNAL(checkedItemsChanged(QStringList)), this, SLOT(checkedItemsChanged(QStringList)));
    }
    return editor_;
}

void QtFlagEditor::setValueToEditor(int value)
{
    QIntList flagValues;
    for(int i = 0; i < flagNames_.size(); ++i)
    {
        if(value & (1 << i))
        {
            flagValues.push_back(i);
        }
    }
    editor_->blockSignals(true);
    editor_->setCheckedIndices(flagValues);
    editor_->blockSignals(false);
}

void QtFlagEditor::onPropertyValueChange(QtProperty *property)
{
    value_ = property->getValue().toInt();
    if(NULL != editor_)
    {
        setValueToEditor(value_);
    }
}

void QtFlagEditor::checkedItemsChanged(const QStringList& /*items*/)
{
    QIntList indices = editor_->checkedIndices();
    int value = 0;
    foreach(int index, indices)
    {
        value |= (1 << index);
    }
    if(value != value_)
    {
        value_ = value;
        property_->setValue(value_);
    }
}

void QtFlagEditor::slotSetAttribute(QtProperty * property, const QString &name)
{
    if(name == QtAttributeName::FlagName)
    {
        editor_->clear();

        flagNames_ = property->getAttribute(QtAttributeName::FlagName).toStringList();
        editor_->addItems(flagNames_);
    }
}

/********************************************************************/
QtBoolEditor::QtBoolEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property_->getValue().toBool();
}

QWidget* QtBoolEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(NULL == editor_)
    {
        editor_ = new QtBoolEdit(parent);
        editor_->setChecked(value_);

        connect(editor_, SIGNAL(toggled(bool)), this, SLOT(slotEditorValueChange(bool)));
    }
    return editor_;
}

void QtBoolEditor::onPropertyValueChange(QtProperty * property)
{
    value_ = property->getValue().toBool();
    if(NULL != editor_)
    {
        editor_->blockSignals(true);
        editor_->setCheckState(value_ ? Qt::Checked : Qt::Unchecked);
        editor_->blockSignals(false);
    }
}

void QtBoolEditor::slotEditorValueChange(bool value)
{
    if(value != value_)
    {
        value_ = value;
        property_->setValue(value_);
    }
}

/********************************************************************/
QtColorEditor::QtColorEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = QtPropertyBrowserUtils::variant2color(property->getValue());
}

QWidget* QtColorEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(NULL == editor_)
    {
        editor_ = new QtColorEditWidget(parent);
        editor_->setValue(value_);

        connect(editor_, SIGNAL(valueChanged(QColor)), this, SLOT(slotEditorValueChange(QColor)));
    }
    return editor_;
}

void QtColorEditor::onPropertyValueChange(QtProperty * property)
{
    value_ = QtPropertyBrowserUtils::variant2color(property->getValue());
    if(NULL != editor_)
    {
        editor_->blockSignals(true);
        editor_->setValue(value_);
        editor_->blockSignals(false);
    }
}

void QtColorEditor::slotEditorValueChange(const QColor &color)
{
    if(color != value_)
    {
        value_ = color;
        property_->setValue(QtPropertyBrowserUtils::color2variant(value_));
    }
}


/********************************************************************/
QtFileEditor::QtFileEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
    , input_(NULL)
    , button_(NULL)
    , dialogType_(READ_FILE)
{
    value_ = property->getValue().toString();
    connect(property_, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtFileEditor::createEditor(QWidget *parent, QtPropertyEditorFactory * /*factory*/)
{
    if(NULL != editor_)
    {
        return editor_;
    }

    editor_ = new QWidget(parent);

    QHBoxLayout *layout = new QHBoxLayout(editor_);
    QtPropertyBrowserUtils::setupTreeViewEditorMargin(layout);
    layout->setSpacing(0);

    input_ = new QLineEdit();
    input_->setText(value_);
    input_->setAcceptDrops(true);
    input_->installEventFilter(this);
    connect(input_, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
    layout->addWidget(input_);

    button_ = new QToolButton();
    button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    button_->setFixedWidth(20);
    button_->setText(tr("..."));
    editor_->setFocusProxy(button_);
    editor_->setFocusPolicy(button_->focusPolicy());
    connect(button_, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
    layout->addWidget(button_);

    slotSetAttribute(property_, QtAttributeName::FileDialogType);
    slotSetAttribute(property_, QtAttributeName::FileDialogFilter);
    slotSetAttribute(property_, QtAttributeName::FileRelativePath);
    slotSetAttribute(property_, QtAttributeName::ReadOnly);

    return editor_;
}

void QtFileEditor::onPropertyValueChange(QtProperty *property)
{
    QString value = property->getValue().toString();
    if(editor_ != NULL)
    {
        setValue(value);
    }
}

void QtFileEditor::slotEditorDestory(QObject *object)
{
    QtPropertyEditor::slotEditorDestory(object);

    input_ = NULL;
    button_ = NULL;
}

void QtFileEditor::slotSetAttribute(QtProperty *property, const QString &name)
{
    QVariant value = property->getAttribute(name);
    if(name == QtAttributeName::FileDialogType)
    {
        if(value.type() == QVariant::Int)
        {
            dialogType_ = (DialogType)value.toInt();
        }
    }
    else if(name == QtAttributeName::FileDialogFilter)
    {
        if(value.type() == QVariant::String)
        {
            filter_ = value.toString();
        }
    }
    else if(name == QtAttributeName::FileRelativePath)
    {
        if(value.type() == QVariant::String)
        {
            relativePath_ = value.toString();
        }
    }
    else if(name == QtAttributeName::ReadOnly)
    {
        if(editor_ != NULL && value.type() == QVariant::Bool)
        {
            input_->setReadOnly(value.toBool());
        }
    }
}

void QtFileEditor::setValue(const QString &value)
{
    value_ = value;

    if(input_)
    {
        input_->setText(value);
    }
}

void QtFileEditor::onFileSeleted(const QString &fullPath)
{
    QString path = fullPath;
    if(!relativePath_.isEmpty())
    {
        path = QDir(relativePath_).relativeFilePath(path);
    }

    if(path != value_)
    {
        setValue(path);
        property_->setValue(path);
    }
}

void QtFileEditor::slotButtonClicked()
{
    QString path;
    switch(dialogType_)
    {
    case READ_FILE:
        path = QFileDialog::getOpenFileName(NULL, tr("open file"), value_, filter_);
        break;

   case WRITE_FILE:
        path = QFileDialog::getSaveFileName(NULL, tr("save file"), value_, filter_);
        break;

    case DIRECTORY:
        path = QFileDialog::getExistingDirectory(NULL, tr("open directory"), value_);
        break;

    default:
        break;
    }

    if(path.isEmpty())
    {
        return;
    }

    onFileSeleted(path);
}

void QtFileEditor::slotEditingFinished()
{
    QString text = input_->text();
    if(text != value_)
    {
        value_ = text;
        property_->setValue(value_);
    }
}

bool QtFileEditor::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == input_)
    {
        if(event->type() == QEvent::DragEnter)
        {
            QDragEnterEvent *dragEvent = (QDragEnterEvent*)event;
            const QMimeData *data = dragEvent->mimeData();
            if(data->hasUrls())
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
            if(data->hasUrls())
            {
                event->accept();
                dropEvent->setDropAction(Qt::CopyAction);

                QString path = data->urls()[0].toLocalFile();
                onFileSeleted(path);
            }
            else
            {
                event->ignore();
            }
            return true;
        }
    }

    return QtPropertyEditor::eventFilter(obj, event);
}


QtDynamicItemEditor::QtDynamicItemEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
    , impl_(NULL)
{

}

QWidget* QtDynamicItemEditor::createEditor(QWidget *parent, QtPropertyEditorFactory *factory)
{
    if(editor_ != NULL)
    {
        return editor_;
    }

    editor_ = new QWidget(parent);

    QHBoxLayout *layout = new QHBoxLayout(editor_);
    layout->setMargin(0);
    layout->setSpacing(1);
    editor_->setLayout(layout);

    QtDynamicItemProperty *property = dynamic_cast<QtDynamicItemProperty*>(property_);
    if(property != NULL && property->getImpl() != NULL)
    {
        impl_ = factory->createPropertyEditor(property->getImpl());
        QWidget *subEditor = impl_->createEditor(editor_, factory);
        if(subEditor != NULL)
        {
            layout->addWidget(subEditor);
            editor_->setFocusProxy(subEditor);
            editor_->setFocusPolicy(subEditor->focusPolicy());
        }
    }

    QToolButton *btnMoveUp = new QToolButton();
    btnMoveUp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    btnMoveUp->setFixedWidth(20);
    btnMoveUp->setText(tr("↑"));
    layout->addWidget(btnMoveUp);

    QToolButton *btnMoveDown = new QToolButton();
    btnMoveDown->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    btnMoveDown->setFixedWidth(20);
    btnMoveDown->setText(tr("↓"));
    layout->addWidget(btnMoveDown);

    QToolButton *btnDel = new QToolButton();
    btnDel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    btnDel->setFixedWidth(20);
    btnDel->setText(tr("X"));
    layout->addWidget(btnDel);

    connect(btnMoveUp, SIGNAL(clicked()), this, SLOT(onBtnMoveUp()));
    connect(btnMoveDown, SIGNAL(clicked()), this, SLOT(onBtnMoveDown()));
    connect(btnDel, SIGNAL(clicked()), this, SLOT(onBtnDelete()));

    return editor_;
}

void QtDynamicItemEditor::onPropertyValueChange(QtProperty * /*property*/)
{

}

void QtDynamicItemEditor::onBtnMoveUp()
{
    QtDynamicItemProperty *property = dynamic_cast<QtDynamicItemProperty*>(property_);
    if(property != NULL)
    {
        emit property->signalMoveUp(property);
    }
}

void QtDynamicItemEditor::onBtnMoveDown()
{
    QtDynamicItemProperty *property = dynamic_cast<QtDynamicItemProperty*>(property_);
    if(property != NULL)
    {
        emit property->signalMoveDown(property);
    }
}

void QtDynamicItemEditor::onBtnDelete()
{
    QtDynamicItemProperty *property = dynamic_cast<QtDynamicItemProperty*>(property_);
    if(property != NULL)
    {
        emit property->signalDelete(property);
    }
}


QtFloatListEditor::QtFloatListEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , size_(0)
{
    size_ = property->getAttribute(QtAttributeName::Size).toInt();
    variantList2Vector(property->getValue().toList(), values_);
}

void QtFloatListEditor::variantList2Vector(const QList<QVariant> &input, QVector<float> &output)
{
    output.clear();

    foreach(const QVariant &val, input)
    {
        output.push_back(val.toFloat());
    }

    while(output.size() < size_)
    {
        output.push_back(0.0f);
    }
}

QWidget* QtFloatListEditor::createEditor(QWidget *parent, QtPropertyEditorFactory *factory)
{
    QWidget *editor = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(editor);
    layout->setSpacing(2);
    layout->setMargin(0);
    editor->setLayout(layout);

    QVector<float> values;
    variantList2Vector(property_->getValue().toList(), values);

    for(int i = 0; i < size_; ++i)
    {
        QDoubleSpinBox *edt = new QDoubleSpinBox(editor);
        edt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        edt->setMinimumWidth(10);
        edt->setButtonSymbols(QDoubleSpinBox::NoButtons);
        layout->addWidget(edt);
        editors_.push_back(edt);

        setEditorAttribute(edt, property_, QtAttributeName::MinValue);
        setEditorAttribute(edt, property_, QtAttributeName::MaxValue);
        setEditorAttribute(edt, property_, QtAttributeName::Decimals);
        setEditorAttribute(edt, property_, QtAttributeName::ReadOnly);

        edt->setValue(values[i]);
        connect(edt, SIGNAL(valueChanged(double)), this, SLOT(slotEditorValueChange(double)));
    }

    if(!editors_.empty())
    {
        editors_.first()->setFocus();
    }
    return editor;
}

void QtFloatListEditor::onPropertyValueChange(QtProperty *property)
{
    QVector<float> values;
    variantList2Vector(property->getValue().toList(), values);

    if(values.size() == values_.size())
    {
        int i = 0;
        for( ; i < values.size(); ++i)
        {
            if(!qFuzzyCompare(values[i], values_[i]))
            {
                break;
            }
        }
        if(i == values.size())
        {
            return;
        }
    }

    values_ = values;
    for(int i = 0; i < size_; ++i)
    {
        editors_[i]->setValue(values_[i]);
    }
}

void QtFloatListEditor::slotEditorValueChange(double value)
{
    QDoubleSpinBox *edt = qobject_cast<QDoubleSpinBox*>(sender());
    if(edt == NULL)
    {
        return;
    }

    int index = editors_.indexOf(edt);
    if(qFuzzyCompare(values_[index], (float)value))
    {
        return;
    }
    values_[index] = (float)value;

    QVariantList values;
    foreach(float val, values_)
    {
        values.push_back(QVariant((double)val));
    }

    property_->setValue(QVariant(values));
}

void QtFloatListEditor::slotSetAttribute(QtProperty *property, const QString &name)
{
    foreach(QDoubleSpinBox *editor, editors_)
    {
        setEditorAttribute(editor, property, name);
    }
}

void QtFloatListEditor::setEditorAttribute(QDoubleSpinBox *editor, QtProperty *property, const QString &name)
{
    if(NULL == editor)
    {
        return;
    }

    QVariant v = property->getAttribute(QtAttributeName::MinValue);
    if(name == QtAttributeName::MinValue)
    {
        int minValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::min();
        editor->setMinimum(minValue);
    }
    else if(name == QtAttributeName::MaxValue)
    {
        int maxValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::max();
        editor->setMaximum(maxValue);
    }
    else if(name == QtAttributeName::Decimals)
    {
        if(v.type() == QVariant::Int)
        {
            editor->setDecimals(v.toInt());
        }
    }
    else if(name == QtAttributeName::ReadOnly)
    {
        editor->setReadOnly(v.type() == QVariant::Bool && v.toBool());
    }
}
