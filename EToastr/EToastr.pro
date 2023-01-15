QT += widgets

#TEMPLATE = lib
DEFINES += TOASTR_LIBRARY

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    EToastr.cpp \
    EWidget.cpp \
    main.cpp

HEADERS += \
    EToastr.h \
    EWidget.h

FORMS += \
    Ui/toastr.ui

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Ui/image.qrc
