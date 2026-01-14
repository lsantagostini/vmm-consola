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
#include <QFile>        // Necesario para leer vm.conf
#include <QTextStream>  // Necesario para leer texto del archivo
#include <QMap>         // Necesario para cruzar datos de estado

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refrescarLista();      // Lanza el comando vmctl status
    void procesarEstado();      // Lee la salida y actualiza la tabla
    void iniciarMaquina();      // Lanza vmctl start
    void detenerMaquina();      // Lanza vmctl stop

private:
    // Función auxiliar interna
    QStringList leerVmConf();   // Lee los nombres de las VMs en /etc/vm.conf

    // Elementos de la interfaz
    QTableWidget *tablaVMs;
    QPushButton *btnRefrescar;
    QPushButton *btnIniciar;
    QPushButton *btnDetener;

    // Procesos
    QProcess *procesoInfo;      // Para consultas
    QProcess *procesoControl;   // Para acciones (start/stop)
};

#endif // MAINWINDOW_H
