#include <iostream>
#include <utility>
#include <vector>

#include <higra/hierarchy/component_tree.hpp>
#include <higra/image/graph_image.hpp>
#include <higra/image/tree_of_shapes.hpp>
#include "tree_of_shapes_edit.h"

#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
    // Image path
    // 3D images not yet supported
    std::string filename = "/home/jmendesforte/implementations/hierarchy_based_weights/ressources/ramen.jpg";

    std::cout << "Building tos\n";
    Tree_of_shapes tos = Tree_of_shapes(filename);
    std::cout << tos.nb_nodes() << " tos nodes\n";

    tos.enrich();
    std::cout << "Enriched.\n";
    cv::Mat im = tos.reconstruct_image();
    // Reconstruct the image from a non-modified ToS: reconstruct the original image
    cv::imwrite("orig.png", im);


    int it = 10;

    // Apply 10 consecutive median filters
    for (int i = 0; i < it; i++)
    {
        process_tree_median(tos);
    }

    // Progressively remove nodes with smaller proper parts
    process_tree_proper_part_bottom_up_consecutive(tos, 10, 100, 1000);

    // Construct the modified image
    im = tos.reconstruct_image();
    cv::imwrite("reconstruction.png", im);

    // Proper part color visualisation
    cv::Mat vis = tos.reconstruct_node_colored_image();
    cv::imwrite("nodes_colored.png", vis);

    return 0;
}
