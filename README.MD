# GlbParser

this project is a little parser for `.glb` files. 

## How it works

Just call `LoadBinaryFile` function with a path to a `.glb` file and it will return you a json corresponding to a `.gltf` file and the binary data of the `.glb` file.
After, call `LoadGltf` function with `.gltf` file and binary data, and it will return you a `GltfData` struct holding all the data parsed. Then, you can do everything you want with it ! Check https://github.com/Anthony-Verdon/scop for an example of what you can do !

## Future

this project will probably be updated in the future. Here is a list of things I want to do:
- separate JsonParser into another submodule
- handle more elements in `.glb` data and link the documentation to it
- add tests with doctest to check if everything work
- ...
- 

the goal of this project is not to be the best glb parser, but one working fine and easy to understand (maybe I should add commentarys for that, and a better doc)
