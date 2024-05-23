# KaSVM

The multilevel support vector machine framework KaSVM.

***This is a refactoring experiment***

+ Original repository: https://github.com/schulzchristian/multilevelSVM
+ Paper: [Faster Support Vector Machines [ALENEX19]](https://arxiv.org/abs/1808.06394) - Sebastian Schlag, Matthias Schmitt, Christian Schulz

## Building

* Build [BayesOpt](https://github.com/rmcantin/bayesopt)

  Requirements: `libboost-dev` `cmake` `g++`

  ```sh
  cd extern/bayesopt
  mkdir build && cd build
  cmake ..
  make -j 4
  cd ../..
  ```

* Build this project
  ```sh
  mkdir build && cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make -j 4
  cd ..
  ```

## Testing

```sh
./build/prepare examples/twonorm.csv
```

```sh
./build/kasvm examples/twonorm
```

## Licences
* [Argtable](https://github.com/jonathanmarvens/argtable2/blob/master/COPYING) - GNU GENERAL PUBLIC LICENSE Version 2
* [Flann](https://github.com/mariusmuja/flann/blob/master/COPYING) - BSD License
* [LibSVM](https://github.com/mljs/libsvm/blob/master/LICENSE) - BSD License
* [ThunderSVM](https://github.com/Xtra-Computing/thundersvm/blob/master/LICENSE) - Apache License 2.0
* [BayesOpt](https://github.com/rmcantin/bayesopt/blob/master/LICENSE) - GNU Affero General Public License v3.0
* [Catch2](https://github.com/catchorg/Catch2/blob/master/LICENSE.txt) - Boost Software License 1.0
