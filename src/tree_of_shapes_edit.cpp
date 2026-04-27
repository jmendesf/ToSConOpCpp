#include "tree_of_shapes_edit.h"

// To each node is given the median value of its neighborhood (parent + children)
void process_tree_median(Tree_of_shapes& tos)
{
    std::queue<Node_tos *> queue;
    queue.push(tos.root);
    std::unordered_map<long, bool> visited;

    while(!queue.empty())
    {
        Node_tos * node = queue.front();
        queue.pop();
        if (!node->root && !node->removed)
        {
            long target_alt = node->get_median_neighbouring_value();
            node = tos.change_alt_of_node(node->name, target_alt);
            if (!node->root && std::abs(node->parent->alt - target_alt) < 1)
            {
                tos.change_alt_of_node(node->name, node->parent->alt);
            }
        }

        for (Node_tos* child : node->children)
        {
            if (child != nullptr)
            {
                if (!visited[child->name])
                {
                    queue.push(child);
                    visited[child->name] = true;
                }
            }
        }
    }
}

// Remove nodes which have a proper part smaller than a certain value
// Top down traversal
void process_tree_proper_part(Tree_of_shapes &tos, long proper_part_value)
{
    std::queue<Node_tos *> queue;
    queue.push(tos.root);
    std::unordered_map<long, bool> visited;

    while(!queue.empty())
    {
        Node_tos * node = queue.front();
        queue.pop();
        if (!node->root)
        {
            while(!node->root && node->proper_part.size() < proper_part_value)
            {
                node = tos.change_alt_of_node(node->name, node->parent->alt);
            }
        }

        for (Node_tos* child : node->children)
        {
            if (child != nullptr)
            {
                if (!visited[child->name])
                {
                    queue.push(child);
                    visited[child->name] = true;
                }
            }
        }
    }
}

// Remove nodes which have a proper part smaller than a certain value
// Bottom up traversal
void process_tree_proper_part_bottom_up(Tree_of_shapes& tos, long proper_part_value)
{
    if (tos.root == nullptr) return;

    std::queue<Node_tos*> queue;
    std::vector<Node_tos*> order;
    std::unordered_map<long, bool> visited;

    queue.push(tos.root);

    while (!queue.empty())
    {
        Node_tos* node = queue.front();
        queue.pop();

        order.push_back(node);

        for (Node_tos* child : node->children)
        {
            if (child != nullptr && !visited[child->name])
            {
                queue.push(child);
            }
        }
    }

    for (auto it = order.rbegin(); it != order.rend(); ++it)
    {
        Node_tos* node = *it;

        if (!node->root && !node->removed)
        {
            while (!node->root && node->proper_part.size() < proper_part_value)
            {
                node = tos.change_alt_of_node(node->name, node->parent->alt);
            }
        }
    }
}


// Progressively remove nodes which have bigger and bigger proper part size until end value is reached
// Bottom up traversal
void process_tree_proper_part_bottom_up_consecutive(Tree_of_shapes &tos, long starting_value, long step, long end_value)
{
    for (int i = starting_value; i < end_value; i += step)    
    {
        process_tree_proper_part_bottom_up(tos, i);
    }
}
