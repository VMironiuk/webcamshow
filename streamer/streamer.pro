TEMPLATE = app
TARGET = ../bin/streamer
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH += /usr/include/gstreamer-1.0 \
               /usr/include/glib-2.0 \
               /usr/lib/i386-linux-gnu/glib-2.0/include \
               mediagraph/graphs \
               mediagraph/bins \
               mediagraph/bins/output \
               mediagraph/bins/input

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gstreamer-1.0 \
                   gstreamer-app-1.0 \
                   gstreamer-plugins-base-1.0 \
                   gstreamer-video-1.0

SOURCES += main.cpp \
    streamer.cpp \
    mediagraph/bins/abstractbin.cpp \
    mediagraph/bins/output/rtmpstreamingbin.cpp \
    mediagraph/bins/output/videopreviewbin.cpp \
    mediagraph/graphs/abstractgraph.cpp \
    mediagraph/graphs/previewgraph.cpp \
    mediagraph/graphs/rtmpgraph.cpp \
    mediagraph/graphs/rtmppreviewgraph.cpp \
    mediagraph/graphs/rtmppreviewvideotestgraph.cpp \
    mediagraph/graphs/rtmpvideotestgraph.cpp \
    mediagraph/graphs/videotestgraph.cpp \
    mediagraph/bins/input/videoinputbin.cpp \
    mediagraph/bins/input/audioinputbin.cpp \
    mediagraph/bins/input/videotestbin.cpp

HEADERS += \
    streamer.h \
    mediagraph/bins/abstractbin.h \
    mediagraph/bins/output/rtmpstreamingbin.h \
    mediagraph/bins/output/videopreviewbin.h \
    mediagraph/graphs/abstractgraph.h \
    mediagraph/graphs/previewgraph.h \
    mediagraph/graphs/rtmpgraph.h \
    mediagraph/graphs/rtmppreviewgraph.h \
    mediagraph/graphs/rtmppreviewvideotestgraph.h \
    mediagraph/graphs/rtmpvideotestgraph.h \
    mediagraph/graphs/videotestgraph.h \
    mediagraph/bins/input/videoinputbin.h \
    mediagraph/bins/input/audioinputbin.h \
    mediagraph/bins/input/videotestbin.h
