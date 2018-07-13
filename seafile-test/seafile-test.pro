TEMPLATE = app

QT = core

CONFIG += link_pkgconfig console
CONFIG -= app_bundle
PKGCONFIG += libseafile

TARGET = seafile-test

HEADERS += \
	seafstatus.h

SOURCES += \
	main.cpp \
	seafstatus.cpp
