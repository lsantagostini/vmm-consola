/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#include "mainwindow.h"
#include <QDebug>
#include <utility>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("OpenBSD VMD Manager");
    resize(750, 450);

    tablaVMs = new QTableWidget(this);
    tablaVMs->setColumnCount(3);
    tablaVMs->setHorizontalHeaderLabels(QStringList() << "Nombre" << "Estado" << "ID / Info");
    tablaVMs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaVMs->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaVMs->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaVMs->setEditTriggers(QAbstractItemView::NoEditTriggers);

    btnRefrescar = new QPushButton("🔄 Refrescar", this);
    btnIniciar = new QPushButton("▶️ Iniciar", this);
    btnDetener = new QPushButton("⏹️ Detener", this);

    btnIniciar->setStyleSheet("color: green; font-weight: bold;");
    btnDetener->setStyleSheet("color: red; font-weight: bold;");

    QHBoxLayout *layoutBotones = new QHBoxLayout();
    layoutBotones->addWidget(btnRefrescar);
    layoutBotones->addWidget(btnIniciar);
    layoutBotones->addWidget(btnDetener);

    QWidget *central = new QWidget();
    QVBoxLayout *layoutMain = new QVBoxLayout(central);
    layoutMain->addWidget(tablaVMs);
    layoutMain->addLayout(layoutBotones);
    setCentralWidget(central);

    procesoInfo = new QProcess(this);
    procesoControl = new QProcess(this);

    connect(btnRefrescar, &QPushButton::clicked, this, &MainWindow::refrescarLista);
    connect(btnIniciar, &QPushButton::clicked, this, &MainWindow::iniciarMaquina);
    connect(btnDetener, &QPushButton::clicked, this, &MainWindow::detenerMaquina);

    connect(procesoInfo, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int, QProcess::ExitStatus){
        procesarEstado();
    });

    refrescarLista();
}

MainWindow::~MainWindow() {}

QStringList MainWindow::leerVmConf() {
    QStringList maquinas;
    QFile archivo("/etc/vm.conf");

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return maquinas;
    }

    QTextStream in(&archivo);
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();
        if (linea.startsWith("vm ") && linea.contains("\"")) {
            int inicio = linea.indexOf("\"") + 1;
            int fin = linea.indexOf("\"", inicio);
            if (fin > inicio) {
                QString nombre = linea.mid(inicio, fin - inicio);
                if (!maquinas.contains(nombre)) {
                    maquinas.append(nombre);
                }
            }
        }
    }
    return maquinas;
}

void MainWindow::refrescarLista() {
    procesoInfo->start("vmctl", QStringList() << "status");
}

void MainWindow::procesarEstado() {
    QStringList todasLasVMs = leerVmConf();

    QByteArray salida = procesoInfo->readAllStandardOutput();
    QString texto = QString::fromLocal8Bit(salida);
    QStringList lineasRunning = texto.split("\n");

    QMap<QString, QString> idsReales;
    QMap<QString, QString> estadosReales;

    for (int i = 1; i < lineasRunning.size(); ++i) {
        QString linea = lineasRunning[i].simplified();
        if (linea.isEmpty()) continue;

        QStringList partes = linea.split(" ");


        if (partes.size() >= 8) {
            QString id = partes[0];
            QString nombre = partes.last();
            QString estado = partes.at(partes.size() - 2);

            estadosReales.insert(nombre, estado);
            idsReales.insert(nombre, id);
        }
    }

    tablaVMs->setRowCount(0);

    for (const QString &nombreVM : std::as_const(todasLasVMs)) {
        int row = tablaVMs->rowCount();
        tablaVMs->insertRow(row);

        QTableWidgetItem *itemNombre = new QTableWidgetItem(nombreVM);
        QTableWidgetItem *itemEstado;
        QTableWidgetItem *itemID;

        QString estadoDetectado = estadosReales.value(nombreVM, "apagada");
        QString idDetectado = idsReales.value(nombreVM, "-");

        if (estadoDetectado == "running") {
            itemEstado = new QTableWidgetItem("🟢 Ejecutando");
            itemEstado->setForeground(QBrush(Qt::darkGreen));
            itemID = new QTableWidgetItem("ID: " + idDetectado);
        }
        else if (estadoDetectado == "stopped") {
            itemEstado = new QTableWidgetItem("⚫ Detenida (Stopped)");
            itemEstado->setForeground(QBrush(Qt::black));
            itemID = new QTableWidgetItem("ID: " + idDetectado);
        }
        else {
            // Si no sale en vmctl status (ni running ni stopped)
            itemEstado = new QTableWidgetItem("⚪ Apagada (Off)");
            itemEstado->setForeground(QBrush(Qt::gray));
            itemID = new QTableWidgetItem("-");
        }

        tablaVMs->setItem(row, 0, itemNombre);
        tablaVMs->setItem(row, 1, itemEstado);
        tablaVMs->setItem(row, 2, itemID);
    }
}

void MainWindow::iniciarMaquina() {
    int fila = tablaVMs->currentRow();
    if (fila < 0) return;

    QString nombre = tablaVMs->item(fila, 0)->text();
    QString estado = tablaVMs->item(fila, 1)->text();

    if (estado.contains("Ejecutando")) {
        QMessageBox::information(this, "Aviso", "La máquina ya está corriendo.");
        return;
    }

    procesoControl->start("vmctl", QStringList() << "start" << nombre);
    procesoControl->waitForFinished();

    QProcess::execute("sleep 1");
    refrescarLista();
}

void MainWindow::detenerMaquina() {
    int fila = tablaVMs->currentRow();
    if (fila < 0) return;

    QString idInfo = tablaVMs->item(fila, 2)->text(); // "ID: 1" o "-"

    if (!idInfo.contains("ID:")) {
        QMessageBox::warning(this, "Error", "La máquina no tiene ID (está apagada o no reportada).");
        return;
    }

    QString id = idInfo.split(" ").last();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmar", "¿Detener la VM ID " + id + "?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        procesoControl->start("vmctl", QStringList() << "stop" << id);
        procesoControl->waitForFinished();

        QProcess::execute("sleep 1");
        refrescarLista();
    }
}
