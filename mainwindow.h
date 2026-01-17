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

// Incluimos la nueva lógica separada
#include "vmm_manager.h"

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
    void abrirConsola(); // <--- Slot Nuevo

private:
    // Instancia del gestor de lógica
    VmmManager manager;

    QTableWidget *tablaVMs;
    QPushButton *btnRefrescar;
    QPushButton *btnIniciar;
    QPushButton *btnDetener;
    QPushButton *btnConsola; // <--- Botón Nuevo

    QProcess *procesoInfo;
    QProcess *procesoControl;
};

#endif // MAINWINDOW_H
