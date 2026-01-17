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
    void initTestCase() {}

    void testParseState()
    {
        VmmManager manager;

        QString simulatedOutput =
            "   ID  PID VCPUS  MAXMEM  CURMEM     TTY        OWNER NAME\n"
            "    1 5626     1    512M    305M   ttyp6     leonardo alpine\n"
            "    2 1234     1    1024M   100M   ttyp7     leonardo debian";

        QMap<QString, VmState> result = manager.parseVmctlOutput(simulatedOutput);

        QVERIFY2(result.contains("alpine"), "VM 'alpine' not detected");
        QCOMPARE(result["alpine"].id, "1");

        QVERIFY2(result.contains("debian"), "VM 'debian' not detected");
        QCOMPARE(result["debian"].id, "2");
    }

    void testReadConfig()
    {
        VmmManager manager;

        QTemporaryFile tempFile;
        if (tempFile.open()) {
            QTextStream out(&tempFile);
            out << "switch \"uplink\" {\n";
            out << "    interface bridge0\n";
            out << "}\n";
            out << "vm \"test_vm\" {\n";
            out << "    memory 512M\n";
            out << "}\n";
            out << "vm \"web_server\" {\n";
            out << "    disable\n";
            out << "}\n";
            tempFile.close();

            QStringList vms = manager.readConfiguration(tempFile.fileName());

            QCOMPARE(vms.size(), 2);
            QVERIFY(vms.contains("test_vm"));
            QVERIFY(vms.contains("web_server"));
        }
    }

    void cleanupTestCase() {}
};

QTEST_MAIN(TestManager)
#include "tst_manager.moc"
