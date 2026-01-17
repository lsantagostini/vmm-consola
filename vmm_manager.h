/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 * ISC License
 */

#ifndef VMM_MANAGER_H
#define VMM_MANAGER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QFile>
#include <QTextStream>

struct VmState {
    QString id;
    QString state;
};

class VmmManager
{
public:
    VmmManager();

    QStringList readConfiguration(const QString &filePath = "/etc/vm.conf");
    QMap<QString, VmState> parseVmctlOutput(const QString &rawOutput);
};

#endif // VMM_MANAGER_H
