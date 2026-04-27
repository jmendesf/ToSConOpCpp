#pragma once

#include "tree_of_shapes.h"

void process_tree_median(Tree_of_shapes& tos);

void process_tree_proper_part(Tree_of_shapes& tos, long proper_part_value);

void process_tree_proper_part_bottom_up(Tree_of_shapes& tos, long proper_part_value);

void process_tree_proper_part_bottom_up_consecutive(Tree_of_shapes& tos, long starting_value, long step, long end_value);
