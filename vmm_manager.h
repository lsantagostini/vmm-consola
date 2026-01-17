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

// Estructura simple para guardar el estado de una VM
struct VmEstado {
    QString id;
    QString estado; // "running", "stopped", etc.
};

class VmmManager
{
public:
    VmmManager();

    // Permite pasar una ruta distinta para facilitar los TESTS
    QStringList leerConfiguracion(const QString &rutaArchivo = "/etc/vm.conf");

    // Recibe el texto crudo de vmctl y devuelve un mapa de estados
    QMap<QString, VmEstado> parsearSalidaVmctl(const QString &salidaRaw);
};

#endif // VMM_MANAGER_H
