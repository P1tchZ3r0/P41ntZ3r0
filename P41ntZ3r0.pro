QT += core gui widgets
CONFIG += c++17

TARGET = P41ntZ3r0
TEMPLATE = app

INCLUDEPATH += Headers Headers/Command

SOURCES += \
  Sources/Command/Commands.cpp \
  Sources/Command/Invoker.cpp \
  Sources/Canvas.cpp \
  Sources/ButtonPanel.cpp \
  Sources/MainWindow.cpp \
  Sources/PushButton.cpp \
  Sources/main.cpp 

HEADERS += \
  Headers/Command/Commands.h \
  Headers/Command/Invoker.h \
  Headers/Canvas.h \
  Headers/ButtonPanel.h \
  Headers/MainWindow.h \
  Headers/PushButton.h

