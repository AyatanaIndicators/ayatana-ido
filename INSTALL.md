# Build and installation instructions

## Compile-time build dependencies

 - cmake (>= 3.13)
 - cmake-extras
 - glib-2.0 (>= 2.36)
 - gobject-introspection
 - gtk-doc
 - vala (>= 0.16)
 - gtk+-3.0 (>= 3.24)
 - gtest (>= 1.6.0)
 - gmock
 - gcovr (>= 2.4)
 - lcov (>= 1.9)

## For end-users and packagers

```
cd ayatana-ido-X.Y.Z
mkdir build
cd build
cmake ..
make
sudo make install
```
## For testers - unit tests only

```
cd ayatana-ido-X.Y.Z
mkdir build
cd build
cmake .. -DENABLE_TESTS=ON
make
make test
```
## For testers - both unit tests and code coverage

```
cd ayatana-ido-X.Y.Z
mkdir build
cd build
cmake .. -DENABLE_COVERAGE=ON
make
make test
make coverage-html
```
**The install prefix defaults to `/usr`, change it with `-DCMAKE_INSTALL_PREFIX=/some/path`**
