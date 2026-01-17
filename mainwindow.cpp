/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 * ISC License
 */

#include "mainwindow.h"
#include <QDebug>
#include <utility>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("OpenBSD VMD Manager"));
    resize(750, 450);

    tablaVMs = new QTableWidget(this);
    tablaVMs->setColumnCount(3);
    // Usamos tr() para traducción
    tablaVMs->setHorizontalHeaderLabels(QStringList()
        << tr("Nombre") << tr("Estado") << tr("ID / Info"));

    tablaVMs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaVMs->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaVMs->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaVMs->setEditTriggers(QAbstractItemView::NoEditTriggers);

    btnRefrescar = new QPushButton(tr("🔄 Refrescar"), this);
    btnIniciar = new QPushButton(tr("▶️ Iniciar"), this);
    btnDetener = new QPushButton(tr("⏹️ Detener"), this);
    btnConsola = new QPushButton(tr("💻 Consola"), this); // <--- Nuevo

    // Estilos
    btnIniciar->setStyleSheet("color: green; font-weight: bold;");
    btnDetener->setStyleSheet("color: red; font-weight: bold;");
    btnConsola->setStyleSheet("font-weight: bold; color: blue;");

    QHBoxLayout *layoutBotones = new QHBoxLayout();
    layoutBotones->addWidget(btnRefrescar);
    layoutBotones->addWidget(btnIniciar);
    layoutBotones->addWidget(btnDetener);
    layoutBotones->addWidget(btnConsola); // <--- Nuevo

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
    connect(btnConsola, &QPushButton::clicked, this, &MainWindow::abrirConsola); // <--- Nuevo

    connect(procesoInfo, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int, QProcess::ExitStatus){
        procesarEstado();
    });

    refrescarLista();
}

MainWindow::~MainWindow() {}

void MainWindow::refrescarLista() {
    procesoInfo->start("vmctl", QStringList() << "status");
}

void MainWindow::procesarEstado() {
    // 1. Usamos el manager para leer config (/etc/vm.conf)
    QStringList todasLasVMs = manager.leerConfiguracion();

    // 2. Leemos la salida del proceso y parseamos con el manager
    QByteArray salida = procesoInfo->readAllStandardOutput();
    QString texto = QString::fromLocal8Bit(salida);

    QMap<QString, VmEstado> infoEstados = manager.parsearSalidaVmctl(texto);

    tablaVMs->setRowCount(0);

    for (const QString &nombreVM : std::as_const(todasLasVMs)) {
        int row = tablaVMs->rowCount();
        tablaVMs->insertRow(row);

        QTableWidgetItem *itemNombre = new QTableWidgetItem(nombreVM);
        QTableWidgetItem *itemEstado;
        QTableWidgetItem *itemID;

        // Valores por defecto
        QString estadoTexto = "apagada";
        QString idTexto = "-";

        if (infoEstados.contains(nombreVM)) {
            estadoTexto = infoEstados.value(nombreVM).estado;
            idTexto = infoEstados.value(nombreVM).id;
        }

        if (estadoTexto == "running") {
            itemEstado = new QTableWidgetItem(tr("🟢 Ejecutando"));
            itemEstado->setForeground(QBrush(Qt::darkGreen));
            itemID = new QTableWidgetItem(tr("ID: ") + idTexto);
        }
        else if (estadoTexto == "stopped") {
            itemEstado = new QTableWidgetItem(tr("⚫ Detenida (Stopped)"));
            itemEstado->setForeground(QBrush(Qt::black));
            itemID = new QTableWidgetItem(tr("ID: ") + idTexto);
        }
        else {
            itemEstado = new QTableWidgetItem(tr("⚪ Apagada (Off)"));
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

    if (estado.contains("🟢")) {
        QMessageBox::information(this, tr("Aviso"), tr("La máquina ya está corriendo."));
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

    QString idInfo = tablaVMs->item(fila, 2)->text();

    if (!idInfo.contains("ID:")) {
        QMessageBox::warning(this, tr("Error"), tr("La máquina no tiene ID (está apagada o no reportada)."));
        return;
    }

    QString id = idInfo.split(" ").last();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirmar"),
                                  tr("¿Detener la VM ID %1?").arg(id),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        procesoControl->start("vmctl", QStringList() << "stop" << id);
        procesoControl->waitForFinished();

        QProcess::execute("sleep 1");
        refrescarLista();
    }
}

void MainWindow::abrirConsola() {
    int fila = tablaVMs->currentRow();
    if (fila < 0) return;

    QString idInfo = tablaVMs->item(fila, 2)->text();

    if (!idInfo.contains("ID:")) {
        QMessageBox::warning(this, tr("Error"),
            tr("La máquina debe estar encendida (con ID) para abrir la consola."));
        return;
    }

    QString id = idInfo.split(" ").last();

    // Lanzamos xterm ejecutando vmctl console
    bool exito = QProcess::startDetached("xterm", QStringList() << "-e" << "vmctl" << "console" << id);

    if (!exito) {
        QMessageBox::critical(this, tr("Error"),
            tr("No se pudo iniciar xterm. Asegúrate de tener 'xterm' instalado."));
    }
}
