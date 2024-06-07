## Instrucciones para ejecutar el código

* Primero que todo, debemos ubicarnos en la ruta donde tenemos los cuatro archivos (3 *.cpp* y 1 *.hpp*). 

* Una vez allí, debemos compilar el archivo *dijkstra-test.cpp* el cual contiene la función *main*, con el siguiente comando:
    ```
    g++ dijkstra-test.cpp -o dijkstra-test.exe
    ```

* Una vez que obtenemos el archivo compilado **.exe**, debemos ejecutarlo con el siguiente comando:
    ```
    ./dijkstra-test.exe
    ```

    Luego, el algoritmo comenzará a ejecutarse e irá imprimiendo en la salida los $(i, j)$ pertenecientes a la cantidad $(2^i, 2^j)$ de vértices y aristas, respectivamente, con los cuales está calculando el algoritmo.

* Finalmente, se creará un archivo llamado **resultados.txt** en el mismo directorio donde están los archivos iniciales (y si ya existe lo sobreescribe), el cual contiene los tiempos que tardó el algoritmo en resolver para cada $(i, j)$, con las 50 iteraciones cada uno, utilizando heap binario y heap de Fibonacci.
