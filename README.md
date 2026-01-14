# OpenBSD VMD Manager

Una interfaz gráfica (GUI) simple y nativa para administrar máquinas virtuales en OpenBSD utilizando `vmd(8)` y `vmctl(8)`.

El objetivo es proveer una herramienta ligera escrita en C++ y Qt 6 que se integre visual y funcionalmente con el sistema base.

## Características

* **Detección inteligente:** Lee `/etc/vm.conf` para listar máquinas configuradas.
* **Estado en tiempo real:** Cruza información con `vmctl status` para mostrar si las VMs están corriendo o detenidas.
* **Control simple:** Iniciar y detener máquinas con un clic.
* **Sin privilegios innecesarios:** Diseñado para gestionar VMs del propio usuario sin requerir `root` ni `doas` para las operaciones básicas.

## Requisitos

* OpenBSD (Probado en 7.8)
* Qt 6 (`qt6-qtbase`)
* CMake

## Instalación de Dependencias

Ejecuta lo siguiente en tu terminal para instalar el compilador y las librerías necesarias:

```ksh
doas pkg_add qt6-qtbase cmake
```
## Compilación
Sigue estos pasos para compilar el proyecto desde el código fuente:


### 1. Crear directorio de compilación
```ksh
mkdir build && cd build
```

### 2. Generar el Makefile con CMake
```ksh
cmake ..
```

### 3. Compilar
```ksh
make
```

### 4. Ejecutar
```ksh
./vmConsola
```
## Licencia

El código fuente de este proyecto está licenciado bajo la Licencia ISC (equivalente a BSD 2-Clause). Consulta el archivo LICENSE para ver el texto completo.

## Créditos de Terceros
Este software utiliza el framework Qt 6, el cual está licenciado bajo la GNU Lesser General Public License (LGPL) version 3.
Qt es propiedad de The Qt Company Ltd. y otros colaboradores.
Más información en [qt.io](https://www.qt.io).
