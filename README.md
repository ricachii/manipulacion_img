# Tarea 1 - Manipulacion de imagenes

Este repo es la entrega del laboratorio 1 de Estructura de Datos. La clase `moving_image` trabaja con matrices RGB y permite mover y rotar la imagen, ademas de manejar historial de movimientos.

## Integrantes

- Nicolas Renato Ricciardi Acuna
- Enzo Gabriel Levancini Arriagada

## Archivos

- `moving_image.h`: clase principal y metodos nuevos.
- `test.cpp`: ejemplo de uso y generacion de imagenes.
- `svpng.inc`, `basics.h`, `small_mario_rgb.h`: base entregada.
- `imagenes/`: salida de los PNG.

## Compilacion

En consola (Windows o Linux):

```bash
g++ -o ejecutable test.cpp
```

Luego ejecutar:

```bash
./ejecutable
```

En Windows tambien puede quedar como `ejecutable.exe`.

## Salida

Las imagenes se guardan en la carpeta `imagenes/`. Si se ejecuta varias veces, el `test.cpp` borra las anteriores antes de generar nuevas.

## Notas

- Se implementaron `move_right`, `move_up`, `move_down`, `rotate`.
- Tambien `undo`, `redo`, `repeat`, `repeat_all` usando pilas y colas.
- `repeat_all` reproduce desde el estado inicial y genera un PNG por movimiento.
