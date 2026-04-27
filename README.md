# Connected operators based on the Tree of Shapes

## Presentation
This library allows the construction and modification of a Tree of Shapes (ToS) [1] based on the modification strategies introduced in [2].
This implementation is ported and updated from the original Python proof-of-concept, which can be found here https://github.com/jmendesf/ToSConOp (deprecated).

This library allows the modification of the ToS such that (1) the resulting structure remains the ToS of the image it reconstructs and (2) the signs of the gradients between the regions of the image remains unchanged before and after modification.

## Prerequisites and installation

OpenCV is required to handle image. 
It can be downloaded and installed following this guide: https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

The ToS is computed using the Higra [3] library, which can be downloaded here: https://github.com/higra/Higra. 

- Download or clone this project;
- Locate the following line in CMakeLists.txt: ``/path/to/Higra/include`` (l.19) and replace it with the actual path to Higra's include/ folder you just downloaded;
- In the root folder of the project, type:

```bash

mkdir build; cd build

cmake ..

make -j4

./edit_tos

```



to compile and execute the code written in ``main.cpp``.


## Examples 

The ``main.cpp`` serves as a temporary example until the command line implementation is fully functional. 

```c++
std::string filename = "/path/to/image.png";
Tree_of_shapes tos = Tree_of_shapes(filename);
```
Creates a ToS from the given filename. 
It can then be modified using the functions defined in ``tree_of_shapes_edit.h``.
For instance, using the following:
```cpp
process_tree_proper_part(tos, 60);
```
Removes nodes with a proper part of size smaller than 60.

The image can then be reconstructed:

```cpp
im = tos.reconstruct_image();
cv::imwrite("reconstruction.png", im);
```

Note that nodes proper part can be somewhat visualized using the following function:

```cpp
cv::Mat vis = tos.reconstruct_node_colored_image();
cv::imwrite("nodes_colored.png", vis);
```

## References 

[1] V. Caselles, B. Coll, and J. Morel, “Topographic maps and local contrast changes in natural images,” International Journal of Computer Vision, vol. 33, pp. 5–27, 1999.

[2] Julien Mendes Forte, Nicolas Passat, Akinobu Shimizu, Yukiko Kenmochi. Consistent connected operators based on trees of shapes. SIAM Journal on Imaging Sciences, 2025, 18 (4), pp.2547-2579. 

[3] Benjamin Perret, Giovanni Chierchia, Jean Cousty, Silvio Jamil F. Guimarães, Yukiko Kenmochi, et al.. Higra: Hierarchical Graph Analysis. SoftwareX, 2019, 10, pp.100335. 