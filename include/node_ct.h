#pragma once
#include <string>
#include <vector>

#include <higra/image/graph_image.hpp>
#include <higra/hierarchy/component_tree.hpp>
#include <opencv2/opencv.hpp>
#include <xtensor/xadapt.hpp>
#include <list>
#include <tuple>
#include <array>
#include <unordered_map>

enum Node_class
{
    MIN_TREE,
    MAX_TREE,
    NA
};

struct Node_ct
{
    Node_ct(int id, int alt, Node_class node_class, Node_ct *parent);
    
    int name;
    int alt;
    bool root = false;
    Node_class node_class;

    Node_ct *parent = nullptr;
    std::list<Node_ct *> children;
    int id_node_tos = -1;
    long id_ct_anchor = -1;
};