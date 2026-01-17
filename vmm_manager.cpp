/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 * ISC License
 */

#include "vmm_manager.h"

VmmManager::VmmManager() {}

QStringList VmmManager::readConfiguration(const QString &filePath)
{
    QStringList vms;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return vms;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("vm ") && line.contains("\"")) {
            int start = line.indexOf("\"") + 1;
            int end = line.indexOf("\"", start);
            if (end > start) {
                QString name = line.mid(start, end - start);
                if (!vms.contains(name)) {
                    vms.append(name);
                }
            }
        }
    }
    return vms;
}

QMap<QString, VmState> VmmManager::parseVmctlOutput(const QString &rawOutput)
{
    QMap<QString, VmState> stateMap;

    QString text = rawOutput;
    QStringList lines = text.split("\n");

    for (int i = 1; i < lines.size(); ++i) {
        QString line = lines[i].simplified();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(" ");

        if (parts.size() >= 8) {
            VmState stateObj;
            stateObj.id = parts[0];
            QString name = parts.last();
            stateObj.state = parts.at(parts.size() - 2);

            stateMap.insert(name, stateObj);
        }
    }
    return stateMap;
}
