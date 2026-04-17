# Sistema de Cajero Automático en C++

Este proyecto simula el funcionamiento de un cajero automático desarrollado en C++. Permite gestionar usuarios, autenticación segura mediante PIN cifrado, y operaciones bancarias básicas utilizando almacenamiento en archivos de texto.

## Funcionalidades

- Inicio de sesión con ID y PIN
- Validación de credenciales
- Bloqueo de cuenta tras 3 intentos fallidos (con temporizador de desbloqueo)
- Consulta de saldo
- Retiro de dinero
- Depósito de dinero
- Cambio de PIN con validaciones
- Persistencia de datos en archivos `.txt`

## Seguridad

- Los PIN no se almacenan en texto plano
- Se utiliza cifrado mediante operación XOR con clave (`CLAVE_CIFRADO`)
- Los datos se guardan en formato hexadecimal en el archivo
- Los PIN se descifran automáticamente al cargar el sistema

## Validaciones implementadas

- ID: solo números, mínimo 6 dígitos
- PIN: mínimo 6 caracteres, al menos:
  - una letra mayúscula
  - una letra minúscula
  - un número
  - sin símbolos
- Montos: validación de números positivos (enteros o decimales)

## Tecnologías utilizadas

- C++
- Manejo de archivos (`.txt`)
- Librerías estándar:
  - stdio.h
  - string.h
  - ctype.h
  - time.h
  - stdlib.h
  - unistd.h

## Estructura del sistema

El sistema utiliza múltiples funciones para modularizar la lógica:

- `inicioSesion()` → autenticación de usuario
- `mostrarMenu()` → navegación principal
- `consultarSaldo()`, `retirarDinero()`, `depositarDinero()`
- `cambiarPIN()` → actualización segura de credenciales
- `cifrarPIN()` → cifrado/descifrado mediante XOR
- `guardarDatos()` / `cargarDatos()` → persistencia en archivo

## Cómo ejecutar

1. Compilar el archivo `.cpp`
2. Ejecutar el programa en consola
3. Asegurarse de que el archivo `usuarios.txt` esté en la misma carpeta

## Usuarios de prueba

Para probar el sistema, podés utilizar los siguientes datos:

ID: 123456  
PIN: Le1234  

ID: 234567  
PIN: Re1234  

ID: 345678  
PIN: De1234  

## Nota

Los usuarios y PIN incluidos son únicamente para fines de prueba.  
El sistema cifra automáticamente los PIN antes de guardarlos, por lo que los valores en el archivo `.txt` no son legibles directamente.

## Objetivo del proyecto

Aplicar conceptos de programación estructurada, manejo de archivos, validaciones de entrada, seguridad básica y control de flujo para simular un sistema real de cajero automático.

---
