wonderland.maze
===================

C++ header only maze generaters and solvers library.

## Note

the project has not tagged release version yet.

## Dependancy

C++ std only

## Example

- [example/example-1.cxx](example/example-1.cxx)

### Building and Usage

- Building
    1. `git clone git@github.com:usagi/wonderland.maze.git`
    1. `cd wonderland.maze`
    1. `mkdir build`
    1. `cd build`
    1. `cmake ..` with if you want `-G Ninja`, `-DCMAKE_CXX_COMPILER=em++`, etc.
    1. `make`, `ninja` or your buid tool.
- Usage
    - Generators: `example/generate_drill_2d 55 33 AA BB CC DD EE`
        - the 1st parameter( 55 ) is width. require odd number.
        - the 2nd parameter( 33 ) is height. require odd number.
        - the 3rd parameter( AA ) is block cell showing string.
        - the 4th parameter( BB ) is road cell showing string.
        - the 5th parameter( CC ) is unkown cell showing string.
        - the 6th parameter( DD ) is start cell showing string.
        - the 7th parameter( EE ) is goal cell showing string.
        - `example/generate_drill_2d > file.txt` if you want save to a file.
    - Solvers: `example/solver_dijkstra_2d < file.txt`

## Support compilers

- clang++ >= 3.3.0
- g++     >= 4.8.2
- em++    >= 1.20.0

## License

[MIT](LICENSE)
    
## Author

Usagi Ito <usagi@WonderRabbitProject.net>
