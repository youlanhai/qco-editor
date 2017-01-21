/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QTPROPERTYBROWSERUTILS_H
#define QTPROPERTYBROWSERUTILS_H

#include "qtpropertyconfig.h"
#include <QMap>
#include <QIcon>
#include <QWidget>
#include <QStringList>

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

class QMouseEvent;
class QCheckBox;
class QLineEdit;
class QLabel;
class QToolButton;
class QPalette;
class QStyle;
class QLayout;

class QtCursorDatabase
{
public:
    QtCursorDatabase();
    void clear();

    QStringList cursorShapeNames() const;
    QMap<int, QIcon> cursorShapeIcons() const;
    QString cursorToShapeName(const QCursor &cursor) const;
    QIcon cursorToShapeIcon(const QCursor &cursor) const;
    int cursorToValue(const QCursor &cursor) const;
#ifndef QT_NO_CURSOR
    QCursor valueToCursor(int value) const;
#endif
private:
    void appendCursor(Qt::CursorShape shape, const QString &name, const QIcon &icon);
    QStringList cursorNames_;
    QMap<int, QIcon> cursorIcons_;
    QMap<int, Qt::CursorShape> valueToCursorShape_;
    QMap<Qt::CursorShape, int> cursorShapeToValue_;
};

class QtPropertyBrowserUtils
{
public:
    static QPixmap brushValuePixmap(const QBrush &b);
    static QIcon brushValueIcon(const QBrush &b);
    static QString colorValueText(const QColor &c);
    static QPixmap fontValuePixmap(const QFont &f);
    static QIcon fontValueIcon(const QFont &f);
    static QString fontValueText(const QFont &f);
    static QIcon drawCheckBox(bool value);
    static QIcon drawIndicatorIcon(const QPalette &palette, QStyle *style);
    static QColor variant2color(const QVariant &value);
    static QVariant color2variant(const QColor &color);
    static void setupTreeViewEditorMargin(QLayout *lt);
};

class QTPROPERTYSHEET_DLL QtBoolEdit : public QWidget {
    Q_OBJECT
public:
    QtBoolEdit(QWidget *parent = 0);

    bool textVisible() const { return textVisible_; }
    void setTextVisible(bool textVisible);

    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);

    bool isChecked() const;
    void setChecked(bool c);

    bool blockCheckBoxSignals(bool block);

Q_SIGNALS:
    void toggled(bool);

public Q_SLOTS:
    void slotToggle(bool checked);

protected:
    void mousePressEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *);

private:
    QCheckBox *checkBox_;
    bool textVisible_;
};

class QTPROPERTYSHEET_DLL QtKeySequenceEdit : public QWidget
{
    Q_OBJECT
public:
    QtKeySequenceEdit(QWidget *parent = 0);

    QKeySequence keySequence() const;
    bool eventFilter(QObject *o, QEvent *e);
public Q_SLOTS:
    void setKeySequence(const QKeySequence &sequence);
Q_SIGNALS:
    void keySequenceChanged(const QKeySequence &sequence);
protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *);
    bool event(QEvent *e);
private slots:
    void slotClearShortcut();
private:
    void handleKeyEvent(QKeyEvent *e);
    int translateModifiers(Qt::KeyboardModifiers state, const QString &text) const;

    int num_;
    QKeySequence keySequence_;
    QLineEdit *lineEdit_;
};

class QTPROPERTYSHEET_DLL QtColorEditWidget : public QWidget
{
    Q_OBJECT

public:
    QtColorEditWidget(QWidget *parent);

    bool eventFilter(QObject *obj, QEvent *ev);

public Q_SLOTS:
    void setValue(const QColor &value);

Q_SIGNALS:
    void valueChanged(const QColor &value);

protected:
    void paintEvent(QPaintEvent *);

private Q_SLOTS:
    void buttonClicked();

private:
    QColor          color_;
    QLabel*         pixmapLabel_;
    QLabel*         label_;
    QToolButton*    button_;
};

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#endif
