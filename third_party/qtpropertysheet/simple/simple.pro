
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtpropertysheet
TEMPLATE = app
DESTDIR = $$PWD/../bin

include(simple.pri)

LIBS += -L$$DESTDIR -lqtpropertysheet
DEPENDPATH += $$PWD/../src
