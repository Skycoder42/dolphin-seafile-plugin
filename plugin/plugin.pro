TEMPLATE = lib

QT += core gui widgets KIOCore

CONFIG += plugin warning_clean exceptions link_pkgconfig

# add seafile, ccnet
PKGCONFIG += libseafile libccnet

#dolphin
LIBS += -ldolphinvcs
INCLUDEPATH += /usr/include/Dolphin/


TARGET = fileviewseafileplugin
VERSION = 1.0.0

DEFINES += QT_DEPRECATED_WARNINGS QT_ASCII_CAST_WARNINGS

HEADERS += \
		fileviewseafileplugin.h

SOURCES += \
		fileviewseafileplugin.cpp

DISTFILES +=

unix {
	target.path = $$[QT_INSTALL_PLUGINS]

	serviceDesc.files += fileviewseafileplugin.desktop
	serviceDesc.path = /usr/share/kservices5/

	INSTALLS += target serviceDesc
}
