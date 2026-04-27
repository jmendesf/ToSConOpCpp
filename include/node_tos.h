#pragma once

#include "node_ct.h"

struct Node_tos
{
    Node_tos(const unsigned int id, int altitude, Node_tos *par);
    
    void enrich(int highest_alt);
    void compute_interval(int highest_alt);
    void compute_node_class();

    // ToS modification functions
    void compute_boundaries();
    void fuse_to_parent();
    Node_tos* change_node_altitude_in_bounds(long target_alt);
    long bound_value(long value);
    bool is_strictly_between_bounds(long value);
    std::vector<struct Node_tos *> get_lower_bound_children();
    std::vector<struct Node_tos *> get_upper_bound_children();

    // Tools
    long get_median_neighbouring_value();
    void compute_area();
    void compute_perimeter_4conn(int width, int height);

    // Gos functions
    void add_ct_parent(Node_tos *parent);
    void add_adj_parent(std::array<int,2> interval, Node_tos *parent);
    Node_tos * get_ct_parent(int alt);

    // Ctos functions
    void sort_adj_holes();

    // Debug functions
    void print_node();
    void print_children();

    // shared across all hierarchies
    unsigned int name;
    long alt;
    bool root = false;
    long area = 0;
    long perimeter = 0;

    Node_class node_class = NA;
    std::vector<long> proper_part;
    std::array<int, 2> interval;

    long upper_bound = -1;
    long lower_bound = -1;

    // Specific to the tree of shapes
    Node_tos *parent = nullptr;
    std::vector<struct Node_tos *> children;

    // Used for modifications
    bool removed = false;

    // Specific to the graph of shapes
    Node_tos *parent_ct;
    std::unordered_map<int, struct Node_tos*> parent_adj;
    std::vector<std::tuple<struct Node_tos*, std::array<int, 2>>> interval_parent_adj;
    std::list<std::tuple<struct Node_tos*, std::array<int, 2>>> children_gos_adj;
    std::list<struct Node_tos *> children_gos_ct;

    // Specific to complete tree of shapes
    bool holes_sorted = false;

    long node_importance = 0;
};
