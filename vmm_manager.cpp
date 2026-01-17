/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 * ISC License
 */

#include "vmm_manager.h"

VmmManager::VmmManager() {}

QStringList VmmManager::leerConfiguracion(const QString &rutaArchivo)
{
    QStringList maquinas;
    QFile archivo(rutaArchivo);

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return maquinas;
    }

    QTextStream in(&archivo);
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();
        // Lógica de parsing básica de vm.conf
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

QMap<QString, VmEstado> VmmManager::parsearSalidaVmctl(const QString &salidaRaw)
{
    QMap<QString, VmEstado> mapaEstados;

    // Normalizamos saltos de línea
    QString texto = salidaRaw;
    QStringList lineas = texto.split("\n");

    // Empezamos en 1 para saltar la cabecera (ID  PID  VCPUS  MAXMEM  ...)
    for (int i = 1; i < lineas.size(); ++i) {
        QString linea = lineas[i].simplified();
        if (linea.isEmpty()) continue;

        QStringList partes = linea.split(" ");

        if (partes.size() >= 8) {
            VmEstado estadoObj;
            estadoObj.id = partes[0];      // ID es la primera columna
            QString nombre = partes.last(); // Nombre es la última
            estadoObj.estado = partes.at(partes.size() - 2); // Estado es anteúltima

            mapaEstados.insert(nombre, estadoObj);
        }
    }
    return mapaEstados;
}
