# OpenBSD VMD Manager

![License](https://img.shields.io/badge/License-ISC-blue.svg)
![Platform](https://img.shields.io/badge/Platform-OpenBSD-orange.svg)
![Framework](https://img.shields.io/badge/Qt-6.0%2B-green.svg)
![CI Status](https://github.com/lsantagostini/vmm-consola/actions/workflows/ci.yml/badge.svg)

Una interfaz gráfica (GUI) simple y nativa para administrar máquinas virtuales en OpenBSD utilizando `vmd(8)` y `vmctl(8)`.

El objetivo es proveer una herramienta ligera, escrita en **C++** y **Qt 6**, que se integre visual y funcionalmente con el sistema base, respetando la filosofía del sistema.

## Características

* **Detección inteligente:** Lee `/etc/vm.conf` para listar las máquinas configuradas.
* **Estado en tiempo real:** Cruza información con `vmctl status` para indicar visualmente si las VMs están corriendo o detenidas.
* **Control simple:** Permite iniciar y detener máquinas con un solo clic.
* **Sin privilegios innecesarios:** Diseñado para gestionar VMs del usuario actual sin requerir `root` ni `doas` para las operaciones cotidianas.

## Requisitos

* **Sistema Operativo:** OpenBSD (Probado en versión 7.8)
* **Librerías:** Qt 6 (`qt6-qtbase`)
* **Herramientas:** CMake, Compilador C++ (clang viene por defecto)

## Instalación

### 1. Instalar Dependencias
Ejecuta lo siguiente en tu terminal para instalar las herramientas necesarias:

```ksh
doas pkg_add qt6-qtbase cmake
```

### 2. Compilación
Sigue estos pasos para compilar el proyecto desde el código fuente:

```ksh
# 1. Crear directorio de compilación para mantener el orden
mkdir build && cd build

# 2. Generar el Makefile con CMake
cmake ..

# 3. Compilar el proyecto
make

# 4. Ejecutar la aplicación
./vmConsola
```

## Licencia

El código fuente de este proyecto está liberado bajo la **Licencia ISC** (funcionalmente equivalente a BSD 2-Clause), alineada con la filosofía de OpenBSD.

Consulta el archivo [LICENSE](LICENSE) para ver el texto completo.

---

### Créditos de Terceros

Este software utiliza el framework **Qt 6**, el cual está licenciado bajo la **GNU Lesser General Public License (LGPL) version 3**.
* Qt es propiedad de The Qt Company Ltd. y otros colaboradores.
* Más información y código fuente en [qt.io](https://www.qt.io).
