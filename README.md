## Quadratic equations solver

A regular quadratic equation solver

### How to build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build 
```

### How to run
It takes a list of quadratic equation parameters

```bash
./build/sources/se_solver 3 4 5 0 4 -4 11 22 123 45 42 42 67 1001 ...
```

and outputs roots and extremes

```bash
(3 4 5) => no roots, Xmin=-0.666667
(0 4 -4) => (1), no extremum
(11 22 123) => no roots, Xmin=-1
(45 42 42) => no roots, Xmin=-0.466667
```
