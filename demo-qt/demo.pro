# LibAAVE auralisation library: demo-qt/demo.pro: QTCreator makefile

INCLUDEPATH += ../libaave
LIBS += ../libaave/libaave.a

# InterSense InertiaCube3 library for the Imu class
INCLUDEPATH += isense
LIBS += -Lisense -lisense64
#LIBS += -Lisense -lisense32

QT += widgets

HEADERS += audio.h geometry.h imu.h view.h
SOURCES += geometry.cpp imu.cpp main.cpp view.cpp

unix {
	LIBS += -lasound
	SOURCES += audio-alsa.cpp
} else {
	QT += multimedia
	SOURCES += audio-multimedia.cpp
}
