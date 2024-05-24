# Forma de compilación y ejecución del código

Para compilar y ejecutar el código de la tarea según el sistema operativo:

## Linux
Para compilar, dirigirse al directorio con todos los archivos y ejecutar el siguiente comando:

```bash
gcc mtree-test.c -o mtree-test -lm
```
Y para ejecutar el código se debe ejecutar el siguiente comando:

```bash
./mtree-test
```

## Windows
Si se quiere compilar y ejecutar en Windows se debe tener instalado un compilador de C. Nosotros utilizamos y recomendamos el software MinGW que tiene el compilador gcc. La forma de instalar el programa y poder ejecutar gccse puede ver en el siguiente tutorial: https://www.youtube.com/watch?v=WM-Y9H2EVzM

Hecho esto, dirigirse al directorio que contiene todos los archivos y ejecutar el siguiente comando:

```bash
gcc mtree-test.c -o mtree-test.exe
```

Y para ejecutar el código se debe ejecutar el siguiente comando:
```bash
.\mtree-test.exe
```

Los archivos adjuntos contienen un ejecutable.

Él código solo se ejecuta para el conjunto más pequeño. Para poder ejecutar para los siguientes conjuntos hay que cambiar la condición de los ciclos for en la parte de "Testing". Para esto se debe hacer lo siguiente:
- Para el método SS, dirigirse a la línea 87 y cambiar el la cota superior del ciclo for por n, donde n es el número de experimentos que se quieren ejecutar (Mientras mayor n, mayor número de puntos).
- Para el método CP realizar lo mismo pero en la línea 105.