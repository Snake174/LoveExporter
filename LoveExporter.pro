QT += core gui network

greaterThan( QT_MAJOR_VERSION, 4 ): QT += widgets
greaterThan( QT_MAJOR_VERSION, 4 ): DEFINES += Qt5

TARGET = LoveExporter
TEMPLATE = app

OBJECTS_DIR = obj
MOC_DIR = obj
RCC_DIR = obj

DEFINES += MAJOR_VERSION=\\\"0\\\"
DEFINES += MINOR_VERSION=\\\"3\\\"

win32 {
  DEFINES += WIN32
  DEFINES += USEWIN32IOAPI
  RC_FILE = ICON.rc
}

SOURCES += \
  main.cpp \
  MainWindow.cpp \
  ExportThread.cpp \
  WindowsExporter.cpp \
  PathUtils.cpp \
  libs/zlib-1.2.8/adler32.c \
  libs/zlib-1.2.8/compress.c \
  libs/zlib-1.2.8/crc32.c \
  libs/zlib-1.2.8/deflate.c \
  libs/zlib-1.2.8/gzclose.c \
  libs/zlib-1.2.8/gzlib.c \
  libs/zlib-1.2.8/gzread.c \
  libs/zlib-1.2.8/gzwrite.c \
  libs/zlib-1.2.8/infback.c \
  libs/zlib-1.2.8/inffast.c \
  libs/zlib-1.2.8/inflate.c \
  libs/zlib-1.2.8/inftrees.c \
  libs/zlib-1.2.8/trees.c \
  libs/zlib-1.2.8/uncompr.c \
  libs/zlib-1.2.8/zutil.c \
  libs/quazip-0.7/JlCompress.cpp \
  libs/quazip-0.7/qioapi.cpp \
  libs/quazip-0.7/quaadler32.cpp \
  libs/quazip-0.7/quacrc32.cpp \
  libs/quazip-0.7/quagzipfile.cpp \
  libs/quazip-0.7/quaziodevice.cpp \
  libs/quazip-0.7/quazip.cpp \
  libs/quazip-0.7/quazipdir.cpp \
  libs/quazip-0.7/quazipfile.cpp \
  libs/quazip-0.7/quazipfileinfo.cpp \
  libs/quazip-0.7/quazipnewinfo.cpp \
  libs/quazip-0.7/unzip.c \
  libs/quazip-0.7/zip.c \
  AndroidExporter.cpp \
  SDKWidget.cpp \
  DownloadManager.cpp \
  TextProgressBar.cpp \
  LinuxExporter.cpp \
  MacExporter.cpp

HEADERS += \
  MainWindow.h \
  ExportThread.h \
  WindowsExporter.h \
  PathUtils.h \
  libs/zlib-1.2.8/crc32.h \
  libs/zlib-1.2.8/deflate.h \
  libs/zlib-1.2.8/gzguts.h \
  libs/zlib-1.2.8/inffast.h \
  libs/zlib-1.2.8/inffixed.h \
  libs/zlib-1.2.8/inflate.h \
  libs/zlib-1.2.8/inftrees.h \
  libs/zlib-1.2.8/trees.h \
  libs/zlib-1.2.8/zconf.h \
  libs/zlib-1.2.8/zlib.h \
  libs/zlib-1.2.8/zutil.h \
  libs/quazip-0.7/crypt.h \
  libs/quazip-0.7/ioapi.h \
  libs/quazip-0.7/JlCompress.h \
  libs/quazip-0.7/quaadler32.h \
  libs/quazip-0.7/quachecksum32.h \
  libs/quazip-0.7/quacrc32.h \
  libs/quazip-0.7/quagzipfile.h \
  libs/quazip-0.7/quaziodevice.h \
  libs/quazip-0.7/quazip.h \
  libs/quazip-0.7/quazip_global.h \
  libs/quazip-0.7/quazipdir.h \
  libs/quazip-0.7/quazipfile.h \
  libs/quazip-0.7/quazipfileinfo.h \
  libs/quazip-0.7/quazipnewinfo.h \
  libs/quazip-0.7/unzip.h \
  libs/quazip-0.7/zip.h \
  AndroidExporter.h \
  SDKWidget.h \
  DownloadManager.h \
  TextProgressBar.h \
  LinuxExporter.h \
  MacExporter.h

RESOURCES += \
  res.qrc

