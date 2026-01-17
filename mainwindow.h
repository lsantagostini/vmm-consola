/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 * ISC License
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
#include <QMap>

#include "vmm_manager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshList();
    void processState();
    void startVm();
    void stopVm();
    void openConsole();

private:
    VmmManager manager;

    QTableWidget *vmTable;
    QPushButton *btnRefresh;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QPushButton *btnConsole;

    QProcess *infoProcess;
    QProcess *controlProcess;
};

#endif // MAINWINDOW_H
