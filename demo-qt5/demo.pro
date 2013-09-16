# AcousticAVE auralisation library
INCLUDEPATH += ../libaave
LIBS += ../libaave/libaave.a

# InterSense InertiaCube3 library for the Imu class
INCLUDEPATH += isense
LIBS += -Lisense -lisense64
#LIBS += -Lisense -lisense32

QT += widgets
QT += multimedia # QAudio*

HEADERS += audio.h geometry.h imu.h view.h
SOURCES += audio.cpp geometry.cpp imu.cpp main.cpp view.cpp
