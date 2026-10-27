/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 * ISC License
 */

#include "mainwindow.h"
#include <QDebug>
#include <QStatusBar>
#include <utility>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("OpenBSD VMD Manager"));
    resize(750, 450);

    vmTable = new QTableWidget(this);
    vmTable->setColumnCount(3);
    vmTable->setHorizontalHeaderLabels(QStringList()
        << tr("Name") << tr("Status") << tr("ID / Info"));

    vmTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    vmTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    vmTable->setSelectionMode(QAbstractItemView::SingleSelection);
    vmTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    btnRefresh = new QPushButton(tr("🔄 Refresh"), this);
    btnStart = new QPushButton(tr("▶️ Start"), this);
    btnStop = new QPushButton(tr("⏹️ Stop"), this);
    btnConsole = new QPushButton(tr("💻 Console"), this);

    btnStart->setStyleSheet("color: green; font-weight: bold;");
    btnStop->setStyleSheet("color: red; font-weight: bold;");
    btnConsole->setStyleSheet("font-weight: bold; color: blue;");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnRefresh);
    buttonLayout->addWidget(btnStart);
    buttonLayout->addWidget(btnStop);
    buttonLayout->addWidget(btnConsole);

    QWidget *central = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->addWidget(vmTable);
    mainLayout->addLayout(buttonLayout);
    setCentralWidget(central);

    infoProcess = new QProcess(this);
    controlProcess = new QProcess(this);

    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshList);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::startVm);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::stopVm);
    connect(btnConsole, &QPushButton::clicked, this, &MainWindow::openConsole);

    connect(infoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int, QProcess::ExitStatus){
        processState();
    });

    connect(controlProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::controlFinished);
    connect(controlProcess, &QProcess::errorOccurred, this, &MainWindow::controlFailed);

    refreshList();
}

MainWindow::~MainWindow() {}

void MainWindow::refreshList() {
    infoProcess->start("vmctl", QStringList() << "status");
}

void MainWindow::processState() {
    bool configOk = false;
    QStringList allVms = manager.readConfiguration("/etc/vm.conf", &configOk);

    if (configOk)
        statusBar()->clearMessage();
    else
        statusBar()->showMessage(tr("Could not read /etc/vm.conf: check that it exists and is readable."));

    QByteArray output = infoProcess->readAllStandardOutput();
    QString text = QString::fromLocal8Bit(output);

    QMap<QString, VmState> states = manager.parseVmctlOutput(text);

    vmTable->setRowCount(0);

    for (const QString &vmName : std::as_const(allVms)) {
        int row = vmTable->rowCount();
        vmTable->insertRow(row);

        QTableWidgetItem *itemName = new QTableWidgetItem(vmName);
        QTableWidgetItem *itemState;
        QTableWidgetItem *itemId;

        QString stateText = "stopped";
        QString idText = "-";

        if (states.contains(vmName)) {
            stateText = states.value(vmName).state;
            idText = states.value(vmName).id;
        }

        if (stateText == "running") {
            itemState = new QTableWidgetItem(tr("🟢 Running"));
            itemState->setForeground(QBrush(Qt::darkGreen));
            itemId = new QTableWidgetItem(tr("ID: ") + idText);
        }
        else if (stateText == "stopped") {
            itemState = new QTableWidgetItem(tr("⚫ Stopped"));
            itemState->setForeground(QBrush(Qt::black));
            itemId = new QTableWidgetItem(tr("ID: ") + idText);
        }
        else {
            itemState = new QTableWidgetItem(tr("⚪ Off"));
            itemState->setForeground(QBrush(Qt::gray));
            itemId = new QTableWidgetItem("-");
        }

        vmTable->setItem(row, 0, itemName);
        vmTable->setItem(row, 1, itemState);
        vmTable->setItem(row, 2, itemId);
    }
}

void MainWindow::startVm() {
    int row = vmTable->currentRow();
    if (row < 0) return;

    QString name = vmTable->item(row, 0)->text();
    QString status = vmTable->item(row, 1)->text();

    if (status.contains("🟢")) {
        QMessageBox::information(this, tr("Info"), tr("The virtual machine is already running."));
        return;
    }

    if (controlProcess->state() != QProcess::NotRunning) return;

    setControlsEnabled(false);
    controlProcess->start("vmctl", QStringList() << "start" << name);
}

void MainWindow::stopVm() {
    int row = vmTable->currentRow();
    if (row < 0) return;

    QString idInfo = vmTable->item(row, 2)->text();

    if (!idInfo.contains("ID:")) {
        QMessageBox::warning(this, tr("Error"), tr("The machine has no ID (it is off or not reported)."));
        return;
    }

    QString id = idInfo.split(" ").last();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm"),
                                  tr("Stop VM ID %1?").arg(id),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (controlProcess->state() != QProcess::NotRunning) return;

        setControlsEnabled(false);
        controlProcess->start("vmctl", QStringList() << "stop" << id);
    }
}

void MainWindow::controlFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    setControlsEnabled(true);

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString detail = QString::fromLocal8Bit(controlProcess->readAllStandardError()).trimmed();
        QMessageBox::warning(this, tr("Error"),
            tr("vmctl failed:\n%1").arg(detail.isEmpty() ? tr("unknown error") : detail));
    }

    refreshList();
}

void MainWindow::controlFailed(QProcess::ProcessError error) {
    // finished() is not emitted when the process never starts
    if (error != QProcess::FailedToStart) return;

    setControlsEnabled(true);
    QMessageBox::critical(this, tr("Error"),
        tr("Could not run vmctl: %1").arg(controlProcess->errorString()));
}

void MainWindow::setControlsEnabled(bool enabled) {
    btnStart->setEnabled(enabled);
    btnStop->setEnabled(enabled);
}

void MainWindow::openConsole() {
    int row = vmTable->currentRow();
    if (row < 0) return;

    QString idInfo = vmTable->item(row, 2)->text();

    if (!idInfo.contains("ID:")) {
        QMessageBox::warning(this, tr("Error"),
            tr("Machine must be running (with ID) to open console."));
        return;
    }

    QString id = idInfo.split(" ").last();

    bool success = QProcess::startDetached("xterm", QStringList() << "-e" << "vmctl" << "console" << id);

    if (!success) {
        QMessageBox::critical(this, tr("Error"),
            tr("Could not start xterm. Please ensure 'xterm' is installed."));
    }
}
