# linux_spi_transport_c

## Notes

This is a program to run in a c environment but is designed to behave like a class. An SPI handle is required the first arg and shouldnt be manually touched. create and destroy the instance with the appropriate functions like in the example and then call the transfer function to complete data exchange.

## Building

1. create a build dir in the root of this repo and cd into it

``` bash 
mkdir build
cd build
```

2. gen build files with cmake

``` bash
cmake ..
```

3. build project

``` bash
make
```

4. run

``` bash
./spi_test
```