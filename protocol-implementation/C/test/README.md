This is the unit test module for the C implementation of the embedded serial protocol. We leverage the google test framework for compartmentalized unit testing for the various modules in the protocol. You can read more about the google test framework here: https://github.com/google/googletest.

You will only need dependencies for the google test framework to run these unit tests - all release versions are passing unit tests.

To run the unit test framework run the following commands out of this directory:
```
rm -rf build
cmake -S . -B build
cmake --build build
./build/protocol_test
```