# Arduino Implementation Examples
All arduino examples are located in the `sketches` folder, and are broken up by example name in their own directories. Currently, because of the nature of arduino include rules, the source code for the protocol implementation is copied and pasted directly into the .ino file. In the future we will add an Arduino implementation folder that can be imported directly as an Arduino library.
## Use
All examples were compiled and tested on an original Arduino Uno board. However the project is configured such that any Arduino with a default Serial communication port will work out of the box. Simply open the `.ino` file in you arduino editor and `verify`, then `upload` your program and you are good to go!
## Examples Explained
### basic_example
This is the basic protocol implementation example. It is configured to work out of the box with the `.example_tester_config.json` configuration of the python serial test program located in `utilities/serial-tester/protocol_tester.py`. For more details on how to use that program see the README for the serial tester program.
