/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 * ISC License
 */

#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        a.installTranslator(&qtTranslator);
    }

    QTranslator myappTranslator;
    if (myappTranslator.load("vmConsola_" + QLocale::system().name())) {
        a.installTranslator(&myappTranslator);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
