# eads-m4-gtest-project

El proyecto contiene un script compile_and_test que compila y ejecuta el test con la opción adecuada para almacenar el resultado de la prueba en disco:
    cmake .
    make
    ./eads-m4-gtest-project --gtest_output=xml

Para realizar el test, ejecutar el siguiente comando desde el directorio raíz del proyecto:
  docker run --rm -v "$PWD":/usr/src/myapp -w /usr/src/myapp ilugi/eads-m4-gtest-project ./compile_and_test
  
Esto añade el directorio actual (la raíz del proyecto) como un volumen, y fija ese directorio como el directorio actual. Después lanza el script de compilación y ejecuta el binario de test resultante. El resultado de las pruebas en el fichero test_detail.xml dentro del mismo volumen.

Se ha implementado el script compile_test porque el siguiente comando provocaba un error en el make:
  docker run --rm -v "$PWD":/usr/src/myapp -w /usr/src/myapp ilugi/eads-m4-gtest-project cmake . && make && ./eads-m4-gtest-project --gtest_output=xml
  
Curiosamente, sin embargo, esa misma línea de comando ejecutada en una shell interactiva dentro del propio contenedor no daba ningún error. 
