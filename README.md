# Virtual-Machine
Basic Virtual Machine in C

This program first unpacks UM files into 32 bit words using bitpacking and stores in big-endian byte ordering.

The 4 most significant bits of each word represent that word's opcode. Opcodes dictate the instruction of each the word and how it interacts with registers and the main memory segment. This machine operates with 8 registers and the 9 least significant bits of each word (3 bits each) represent the indices(0-7) of the active registers for the current instruction.

-Download or clone my repository

Program can be compiled by running: ```bash compile```

The UM can be run using any of the .um or .umz files in the example folder by running: ```./um examples/<fileName>```
