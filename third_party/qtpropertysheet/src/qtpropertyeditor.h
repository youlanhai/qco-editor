#ifndef QTPROPERTYEDITOR_H
#define QTPROPERTYEDITOR_H

#include "qtpropertyconfig.h"
#include <QObject>
#include <QColor>
#include <QList>
#include <QVariant>
#include <QVector>

class QtProperty;
class QWidget;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QToolButton;

class QtColorEditWidget;
class QtBoolEdit;
class QxtCheckComboBox;

class QtPropertyEditorFactory;

// QtPropertyEditor will be destroied when QtEditor destroied.
class QTPROPERTYSHEET_DLL QtPropertyEditor : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyEditor(QtProperty *property);
    virtual ~QtPropertyEditor();

    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory) = 0;

public slots:
    virtual void onPropertyValueChange(QtProperty *property) = 0;
    virtual void onPropertyDestory(QObject *object);
    virtual void slotEditorDestory(QObject *object);

protected:

    QtProperty*         property_;
};


class QTPROPERTYSHEET_DLL QtIntSpinBoxEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtIntSpinBoxEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void slotEditorValueChange(int value);
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    int                 value_;
    QSpinBox*           editor_;

};

class QTPROPERTYSHEET_DLL QtDoubleSpinBoxEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtDoubleSpinBoxEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void slotEditorValueChange(double value);
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    double              value_;
    QDoubleSpinBox*     editor_;

};

class QTPROPERTYSHEET_DLL QtStringEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtStringEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void slotEditFinished();
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    QString             value_;
    QLineEdit*          editor_;
};

class QTPROPERTYSHEET_DLL QtEnumEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtEnumEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    virtual void slotEditorValueChange(int index);
    virtual void slotSetAttribute(QtProperty *property, const QString &name);

private:
    int                 value_;
    QComboBox*          editor_;
};

class QTPROPERTYSHEET_DLL QtEnumPairEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtEnumPairEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    virtual void slotEditorValueChange(int index);
    virtual void slotSetAttribute(QtProperty *property, const QString &name);

protected:
    int                 index_;
    QVariantList        enumValues_;
    QComboBox*          editor_;
};

class QTPROPERTYSHEET_DLL QtFlagEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtFlagEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

    void setValueToEditor(int value);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void checkedItemsChanged(const QStringList& items);
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    int                 value_;
    QxtCheckComboBox*   editor_;
    QStringList         flagNames_;
};

class QTPROPERTYSHEET_DLL QtBoolEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtBoolEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    virtual void slotEditorValueChange(bool value);

protected:
    bool                value_;
    QtBoolEdit*         editor_;
};

class QTPROPERTYSHEET_DLL QtColorEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtColorEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    virtual void slotEditorValueChange(const QColor &color);

protected:
    QColor              value_;
    QtColorEditWidget*  editor_;
};

class QTPROPERTYSHEET_DLL QtFileEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    enum DialogType
    {
        READ_FILE,
        WRITE_FILE,
        DIRECTORY,
    };

    explicit QtFileEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void slotButtonClicked();
    virtual void slotEditingFinished();

    virtual void onPropertyValueChange(QtProperty *property);
    virtual void slotEditorDestory(QObject *object);
    virtual void slotSetAttribute(QtProperty *property, const QString &name);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void setValue(const QString &value);
    virtual void onFileSeleted(const QString &fullPath);

    QString         value_;
    QWidget*        editor_;
    QLineEdit*      input_;
    QToolButton*    button_;
    DialogType      dialogType_;
    QString         filter_;
    QString         relativePath_;
};

class QTPROPERTYSHEET_DLL QtDynamicItemEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtDynamicItemEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);

    void onBtnMoveUp();
    void onBtnMoveDown();
    void onBtnDelete();

private:
    QWidget* 		editor_;
    QtPropertyEditor* impl_;
};

class QTPROPERTYSHEET_DLL QtFloatListEditor : public QtPropertyEditor
{
    Q_OBJECT
public:
    explicit QtFloatListEditor(QtProperty *property);
    virtual QWidget* createEditor(QWidget *parent, QtPropertyEditorFactory *factory);

public slots:
    virtual void onPropertyValueChange(QtProperty *property);
    void slotEditorValueChange(double value);
    void slotSetAttribute(QtProperty *property, const QString &name);

private:
    void setEditorAttribute(QDoubleSpinBox *editor, QtProperty *property, const QString &name);
    void variantList2Vector(const QList<QVariant> &input, QVector<float> &output);

    int                 size_;
    QVector<float>      values_;
    QVector<QDoubleSpinBox*> editors_;
};

#endif // QTPROPERTYEDITOR_H
