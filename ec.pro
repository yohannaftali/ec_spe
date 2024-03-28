#-------------------------------------------------
#
# Project created by QtCreator 2014-08-19T11:18:53
#
#-------------------------------------------------

QT += core gui network serialport sql multimedia widgets printsupport

TARGET = serial
TEMPLATE = app
#QMAKE_LFLAGS+=-static-libgcc
QMAKE_CXXFLAGS += -static -static-libgcc -static-libstdc++

#Setelah deploy menggunakan windeployexe, copy ke-3 file ini ke directory release
#COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/libgcc_s_seh-1.dll ${CMAKE_BINARY_DIR}/application
#COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/libstdc++-6.dll ${CMAKE_BINARY_DIR}/application
#COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/libwinpthread-1.dll ${CMAKE_BINARY_DIR}/application

SOURCES += main.cpp\
        mainwindow.cpp \
        serverwtb.cpp \
        console.cpp \
        settingsdialog.cpp \
        servicedialog.cpp \
        csecuresocket.cpp \
        downloadmanager.cpp

HEADERS  += mainwindow.h \
         serverwtb.h \
         console.h \
         settingsdialog.h \
         servicedialog.h \
         csecuresocket.h \
         downloadmanager.h

FORMS    += mainwindow.ui \
         settingsdialog.ui \
         servicedialog.ui

RESOURCES += \
    resources.qrc

