#pragma once
#include <string>
#include <vector>

#include <higra/image/tree_of_shapes.hpp>
#include "node_tos.h"
#include <opencv2/opencv.hpp>
#include <xtensor/xadapt.hpp>
#include <queue>

struct Tree_of_shapes
{
    Tree_of_shapes(const std::string &filename);
    void pre_process_tos();

    void enrich();
    void compute_area();
    void compute_perimeter();

    // ToS modification functions
    Node_tos * change_alt_of_node(unsigned int node_name, long new_alt);

    // Image reconstruction
    cv::Mat reconstruct_image();
    cv::Mat reconstruct_node_colored_image();

    // Debugging functions
    void print_parents() const;
    void traverse_tree();
    long nb_nodes();
    
    cv::Mat image;
    long img_size;
    int highest_value;
    
    hg::node_weighted_tree<hg::tree, hg::array_1d<uchar>> tos;
    hg::array_1d<hg::index_t> parents;

    Node_tos *root = nullptr;
    std::unordered_map<long, std::unique_ptr<Node_tos>> nodes;
    std::unordered_map<long, std::vector<long>> proper_parts;
    
    bool enriched = false;
};