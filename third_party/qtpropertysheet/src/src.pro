
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtpropertysheet
TEMPLATE = lib
DESTDIR = $$PWD/../bin

# build shared library
DEFINES += QTPROPERTYSHEET_SHARED

# build static library
# DEFINES += QTPROPERTYSHEET_STATIC

include(qtpropertysheet.pri)
