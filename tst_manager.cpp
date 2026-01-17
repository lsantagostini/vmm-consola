/*
 * Copyright (c) 2026 Leonardo Santagostini <leonardo@santagostini.com.ar>
 * ISC License
 */

#include <QtTest>
#include <QTemporaryFile>
#include "vmm_manager.h"

class TestManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Se ejecuta al inicio de todas las pruebas
    }

    void testParsearEstado()
    {
        VmmManager manager;

        // Simulamos una salida limpia de OpenBSD (8 columnas exactas)
        QString salidaSimulada =
            "   ID  PID VCPUS  MAXMEM  CURMEM     TTY        OWNER NAME\n"
            "    1 5626     1    512M    305M   ttyp6     leonardo alpine\n"
            "    2 1234     1    1024M   100M   ttyp7     leonardo debian";

        // Ejecutamos la función a probar
        QMap<QString, VmEstado> resultado = manager.parsearSalidaVmctl(salidaSimulada);

        // Verificaciones
        // 1. Caso Alpine
        QVERIFY2(resultado.contains("alpine"), "La VM 'alpine' no fue detectada");
        QCOMPARE(resultado["alpine"].id, "1");

        // 2. Caso Debian (Ahora debería pasar porque quitamos el guion extra)
        QVERIFY2(resultado.contains("debian"), "La VM 'debian' no fue detectada");
        QCOMPARE(resultado["debian"].id, "2");
    }

    void testLeerConfig()
    {
        VmmManager manager;

        QTemporaryFile archivoTemp;
        if (archivoTemp.open()) {
            QTextStream out(&archivoTemp);
            out << "switch \"uplink\" {\n";
            out << "    interface bridge0\n";
            out << "}\n";
            out << "vm \"maquina_prueba\" {\n";
            out << "    memory 512M\n";
            out << "}\n";
            out << "vm \"servidor_web\" {\n";
            out << "    disable\n";
            out << "}\n";
            archivoTemp.close();

            QStringList maquinas = manager.leerConfiguracion(archivoTemp.fileName());

            QCOMPARE(maquinas.size(), 2);
            QVERIFY(maquinas.contains("maquina_prueba"));
            QVERIFY(maquinas.contains("servidor_web"));
        }
    }

    void cleanupTestCase()
    {
        // Limpieza final
    }
};

QTEST_MAIN(TestManager)
#include "tst_manager.moc"
