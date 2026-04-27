#include "node_tos.h"
#include <iostream>

Node_tos::Node_tos(const unsigned int id, int altitude, Node_tos *par)
    : alt(altitude), name(id), parent(par), children({}), parent_ct(nullptr), parent_adj({}), children_gos_adj({}), children_gos_ct({}), interval({-1, -1}), interval_parent_adj({})
{
    if (par != nullptr)
    {
        par->children.push_back(this);
    }
}

// Compute node class and interval
void Node_tos::enrich(int highest_alt)
{
    // Node class
    compute_node_class();
    // Interval
    compute_interval(highest_alt);
}

void Node_tos::compute_interval(int highest_alt)
{
    if (root)
    {
        interval[0] = highest_alt + 1;
        interval[1] = alt + 1;
    }
    else if (node_class == parent->node_class)
    {
        if (node_class == MAX_TREE)
        {
            interval[0] = parent->alt + 1;
            interval[1] = alt;
        }
        else
        {
            interval[0] = parent->alt - 1;
            interval[1] = alt + 1;
        }
    }
    else
    {
        interval[0] = parent->interval[1];
        interval[1] = alt + 1;
    }
}

void Node_tos::compute_node_class()
{
    if (root)
    {
        node_class = MIN_TREE;
    }
    else if (parent->node_class == MAX_TREE)
    {
        if (alt > parent->alt)
        {
            node_class = MAX_TREE;
        }
        else
        {
            node_class = MIN_TREE;
        }
    }
    else if (parent->node_class == MIN_TREE)
    {
        if (alt < parent->alt)
        {
            node_class = MIN_TREE;
        }
        else
        {
            node_class = MAX_TREE;
        }
    }
}

void Node_tos::compute_boundaries()
{
    long parent_alt = parent != nullptr ? parent->alt : alt;
    upper_bound = -1;
    lower_bound = -1;

    for (auto child : children) 
    {
        // Compute for children
        if (child->alt > alt)
        {
            if (upper_bound == -1 || child->alt < upper_bound) 
            {
                upper_bound = child->alt;
            } 
        } else if (child->alt < alt) {
            if (lower_bound == -1 || child->alt > lower_bound) 
            {
                lower_bound = child->alt;
            }
        }
    }

    // Compare with parent
    if (parent_alt > alt)
    {
        if (upper_bound == -1 || upper_bound > parent_alt)
        {
            upper_bound = parent_alt;
        }
    } else if (parent_alt < alt)
    {
        if (lower_bound == -1 || lower_bound < parent_alt)
        {
            lower_bound = parent_alt;
        }
    }

    if (lower_bound == -1 && alt == 0) {
        lower_bound = 0;
    }
    
}

void Node_tos::fuse_to_parent()
{   
    // for each child, remove the relation to self and link child to parent
    for(auto child : children)
    {
        child->parent = parent;
        parent->children.push_back(child);
    }
    children.clear();

    // proper part is given to parent
    parent->proper_part.reserve(parent->proper_part.size() + proper_part.size());
    parent->proper_part.insert(parent->proper_part.end(), proper_part.begin(), proper_part.end());

    int index = 0;
    for (auto child : parent->children)
    {
        if (child->name == name) {
            parent->children.erase(parent->children.begin() + index);
        }
        index++;
    }
    parent = nullptr;
    removed = true;
}

// Returns a value between the nodes bounds
long Node_tos::bound_value(long value)
{
    if (value < lower_bound && lower_bound != -1)
    {
        return lower_bound;
    } else if (value > upper_bound && upper_bound != -1)
    {
        return upper_bound;
    }
    
    if (root) 
    {
        if (value <= alt) 
        {
            return alt;
        } else {
            return upper_bound;
        }
    }

    return value;
}

bool Node_tos::is_strictly_between_bounds(long value)
{
    return ((value > alt && (value < upper_bound || upper_bound == -1)) ||
            (value < alt && (value > lower_bound || lower_bound == -1)));
}

std::vector<Node_tos *> Node_tos::get_lower_bound_children() {
    std::vector<struct Node_tos *> lower_bound_children;

    if (lower_bound == -1) 
    {
        return lower_bound_children;
    }

    for (auto child : children) 
    {
        if (child->alt == lower_bound) 
        {
            lower_bound_children.push_back(child);
        }
    }

    return lower_bound_children;
}

std::vector<Node_tos *> Node_tos::get_upper_bound_children() {
    std::vector<struct Node_tos *> upper_bound_children;

    if (upper_bound == -1) 
    {
        return upper_bound_children;
    }

    for (auto child : children)
    {
        if (child->alt == upper_bound)
        {
            upper_bound_children.push_back(child);
        }
    }

    return upper_bound_children;
}

// Change the node's altitude. Depending on the node's configuration,
// this operation can lead to fusing with the parent (hence removal of the node) and/or the children nodes.
// Returns:
// If the node is not removed by the altitude change, returns itself
// Else return its parent
Node_tos* Node_tos::change_node_altitude_in_bounds(long target_alt)
{
    if (alt == target_alt) 
    {
        return this;
    }
    
    long parent_alt = parent != nullptr ? parent->alt : alt;
    compute_boundaries();

    // if the new altitude is strictly between the lower and upper bound of the node,
    // there is no impact on the relations of the node
    if (is_strictly_between_bounds(target_alt))
    {
        alt = target_alt;
        return this;
    }

    // Bounding of the new altitude
    // -1 for lower/upper bounds represents infinite
    // new_alt must remain between the lower and upper bounds of the node
    long bounded_alt = bound_value(target_alt);

    // Look for impacted children (i.e. M+ / M-)
    std::vector<struct Node_tos *> impacted_children;
    if (target_alt == lower_bound) 
    {
        impacted_children = get_lower_bound_children();
    } else 
    {
        impacted_children = get_upper_bound_children();
    }

    // If the new altitude of the self node reaches some of its children,
    // these children fuse with the self node and are removed.
    // The children of the fused nodes are now children of the self node
    for (auto impacted_child : impacted_children) 
    {
        impacted_child->fuse_to_parent();
    }

    alt = bounded_alt;

    // If the new altitude of the self node reaches its parent node, the parent absorbs the self node,   
    // and each child of the self node is now a child of the parent node
    if (bounded_alt == parent_alt)
    {
        Node_tos * parent_tmp = parent;
        fuse_to_parent();
        return parent_tmp;
    }

    return this;
}

// returns the median of a long vector
long median(std::vector<long> &v)
{
    size_t n = v.size();
    size_t mid = n/2;

    std::nth_element(v.begin(), v.begin() + mid, v.end());

    if (n % 2 == 1)
    {
        return v[mid];
    } else 
    {
        long a = v[mid];
        std::nth_element(v.begin(), v.begin() + mid - 1, v.end());
        long b = v[mid - 1];
        
        return (a + b) / 2.0;
    }
}

long Node_tos::get_median_neighbouring_value()
{
    std::vector<long> alts = {alt};
    if (!root)
    {
        alts.push_back(parent->alt);
    }
    for (auto child : children)
    {
        alts.push_back(child->alt);
    }
    return median(alts);
}


static std::unordered_set<long> get_area_pixels(const Node_tos* node)
{
    std::unordered_set<long> region;

    std::vector<const Node_tos*> stack;
    stack.push_back(node);

    while (!stack.empty())
    {
        const Node_tos* cur = stack.back();
        stack.pop_back();

        for (long p : cur->proper_part)
            region.insert(p);

        for (const Node_tos* child : cur->children)
            stack.push_back(child);
    }

    return region;
}

void Node_tos::compute_area()
{
    for (long p : proper_part)
    {
        area += 1;
    }
    
    for (Node_tos* child : children)
    {   
        child->compute_area();
        area += child->area;
    }
}

void Node_tos::compute_perimeter_4conn(int width, int height)
{
    std::unordered_set<long> area = get_area_pixels(this);

    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    for (long px : area)
    {
        int y = px / width;
        int x = px % width;

        for (int k = 0; k < 4; k++)
        {
            int nx = x + dx[k];
            int ny = y + dy[k];
            
            if (nx < 0 || nx >= width || ny < 0 || ny >= height)
            {
                perimeter++;
                continue;
            }

            long nidx = ny * width + nx;
            if (!area.contains(nidx))
            {
                perimeter++;
            }
        }
    }
}

void Node_tos::add_ct_parent(Node_tos *parent)
{
    parent_ct = parent;
    parent->children_gos_ct.push_back(this);
}

void Node_tos::add_adj_parent(std::array<int, 2> adj_interval, Node_tos *adj_parent)
{
    if (node_class == MAX_TREE)
    {
        for (int i = adj_interval[0]; i < adj_interval[1] + 1; i++)
        {
            parent_adj[i] = adj_parent;
        }
        std::array<int, 2> interv = {adj_interval[0], adj_interval[1]};
        interval_parent_adj.emplace_back(adj_parent, interv);
    }
    else
    {
        for (int i = adj_interval[1]; i > adj_interval[1] - 1; i--)
        {
            parent_adj[i] = adj_parent;
        }
        std::array<int, 2> interv = {adj_interval[1], adj_interval[0]};
        interval_parent_adj.emplace_back(adj_parent, interv);
    }
    std::array<int, 2> mirrored_interval = {adj_interval[1], adj_interval[0]};
    adj_parent->children_gos_adj.emplace_back(this, mirrored_interval);
}

Node_tos *Node_tos::get_ct_parent(int alt)
{
    if (root)
    {
        return this;
    }
    if (alt == interval[0])
    {
        return parent_ct;
    }
    if (node_class == MAX_TREE)
    {
        if (interval[0] < alt && interval[1] >= alt)
        {
            return this;
        }
    }
    else
    {
        if (interval[0] > alt && interval[1] <= alt)
        {
            return this;
        }
    }

    return nullptr;
}

void Node_tos::sort_adj_holes()
{
    if (holes_sorted) 
    {
        return;
    }
    if (node_class == MAX_TREE)
    {
        children_gos_adj.sort(
            [](const auto &a, const auto &b)
            {
                return std::get<1>(a)[0] < std::get<1>(b)[0];
            });
    }
    else
    {
        children_gos_adj.sort(
            [](const auto &a, const auto &b)
            {
                return std::get<1>(a)[0] > std::get<1>(b)[0];
            });
    }
    holes_sorted = true;
}

// debugging
void Node_tos::print_children()
{
    if (children.size() == 0)
    {
        std::cout << "Node " << name << " is leaf." << std::endl;
        return;
    }
    std::cout << "Child of " << name << ":" << std::endl;
    for (auto &child : children)
    {
        child->print_node();
    }
}

// debugging
void Node_tos::print_node()
{
    if (parent == nullptr)
    {
        std::cout << "Node " << name << " alt " << alt << " parent " << "gouzi" << std::endl;
    }
    else
    {
        std::cout << "Node " << name << " alt " << alt << " parent " << parent->name << std::endl;
    }
}