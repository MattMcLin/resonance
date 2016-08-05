QT += widgets gui multimedia

TARGET = morse_decoder
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    decoder.cpp \
    pulsesampler.cpp \
    pulseplayer.cpp

HEADERS  += dialog.h \
    decoder.h \
    pulsesampler.h \
    pulseplayer.h

FORMS    += dialog.ui

RESOURCES += \
    morse_decoder.qrc
