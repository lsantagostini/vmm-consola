/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProcess>
#include <QHeaderView>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QMap>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refrescarLista();
    void procesarEstado();
    void iniciarMaquina();
    void detenerMaquina();

private:
    QStringList leerVmConf();

    QTableWidget *tablaVMs;
    QPushButton *btnRefrescar;
    QPushButton *btnIniciar;
    QPushButton *btnDetener;

    QProcess *procesoInfo;
    QProcess *procesoControl;
};

#endif // MAINWINDOW_H
