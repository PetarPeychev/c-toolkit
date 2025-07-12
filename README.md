# c-toolkit

My attempt to create a C99 custom standard library and project template to get projects
started quickly without reinventing the wheel.

## Features
**Library**
- [x] Custom memory allocators
- [x] Arena (bump allocator)
- [x] Unit testing framework
- [x] Length-based strings and slices
- [ ] Generic dynamic arrays
- [ ] Generic hashmaps
- [ ] Generic hashsets

**Project Template**
- [x] VS Code configuration
- [ ] Debug, release, and test scripts
- [ ] Valgrind support

## Usage
Clone the repository and start coding in src/ and main/. You can keep the library tests
if you plan to modify the library itself, comment them out in main/test_main.c or 
just remove them entirely. 

Unless you are building a massive project I suggest keeping it a unity build. Simply
include all your .c files in src/main.c and don't write any header files unless you need
to forward-declare due to a circular definition. Include src/main.c in your tests.

## Why not just use Odin, Zig, Jai, etc?
I want the flexibility of having access to the library code without it being part of this
heavy dependency which these newer compilers are. They are also all still pre-1.0 and
potentially unstable. The bus-factor is also high for them, being chiefly maintained by
a single person and with only one compiler implementation. For some of them, I disagree
with some design decisions and believe they go too far in the direction of "massive
magical toolbox". For all its faults, C is still the lingua franca of software, but I do
look forward to seeing these projects improve and gain more traction in the future.
