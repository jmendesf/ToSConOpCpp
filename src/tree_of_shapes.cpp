#include "tree_of_shapes.h"
#include <iostream>
#include <chrono>

Tree_of_shapes::Tree_of_shapes(const std::string &filename)
{
    // Load grayscale
    image = cv::imread(filename, cv::IMREAD_GRAYSCALE);
    if (image.empty())
    {
        throw std::runtime_error("Failed to load image: " + filename);
    }

    img_size = image.rows * image.cols;

    // img to xtensor
    auto img_xt = xt::adapt(
        image.data,
        static_cast<size_t>(image.total()),
        xt::no_ownership(),
        std::vector<size_t>{(size_t)image.rows, (size_t)image.cols});

    // Higra tos computation
    tos = hg::component_tree_tree_of_shapes_image(img_xt);

    tos.tree.compute_children();
    parents = tos.tree.parents();

    // Higra Tos to this
    pre_process_tos();
}

void Tree_of_shapes::pre_process_tos()
{
    // init root
    long id_root = tos.tree.root() - img_size;
    int alt_root = tos.altitudes[tos.tree.root()];
    nodes[id_root] = std::make_unique<Node_tos>(id_root, alt_root, nullptr);
    root = nodes[id_root].get();
    root->root = true;
    highest_value = root->alt;

    // init rest of the tree
    std::vector<long> queue = {id_root};
    while (!queue.empty())
    {
        long current = queue.back();
        queue.pop_back();
        
        Node_tos *current_node = nodes[current].get();
        for (auto child : tos.tree.children(current + img_size))
        {
            if (child < img_size)
            {
                current_node->proper_part.emplace_back(child);
                // proper_parts[current].emplace_back(child);
                continue;
            }

            long id_child = child - img_size;
            int child_alt = tos.altitudes[child];
        
            nodes[id_child] = std::make_unique<Node_tos>(id_child, child_alt, current_node);
            if (nodes[id_child].get()->alt > highest_value) {
                highest_value =  nodes[id_child].get()->alt;
            }
            // Node_tos *child_node = nodes[id_child].get();
            queue.push_back(child - img_size);
        }
    }
}

void Tree_of_shapes::enrich()
{ 
    std::vector<long> stack = {root->name};

    while (!stack.empty())
    {
        long current_id = stack.back();
        stack.pop_back();

        Node_tos *current = nodes[current_id].get();

        // Enriching node if necessary
        if (!enriched)
        {
            current->enrich(highest_value);
        }

        for (auto child : current->children)
        {
            stack.push_back(child->name);
        }
    }

    enriched = true;
}

void Tree_of_shapes::compute_area()
{
    root->compute_area();
}

void Tree_of_shapes::compute_perimeter()
{
    std::queue<Node_tos *> queue;
    queue.push(root);

    while(!queue.empty())
    {
        Node_tos * node = queue.front();
        queue.pop();
        node->compute_perimeter_4conn(image.size().width, image.size().height);

        for (Node_tos* child : node->children)
        {
            if (child != nullptr)
            {
                queue.push(child);
            }
        }
    }
}

Node_tos * Tree_of_shapes::change_alt_of_node(unsigned int node_name, long new_alt)
{
    Node_tos * node = nodes[node_name].get();
    if (node) 
    {
        while(node->alt != new_alt)
        {
            node = node->change_node_altitude_in_bounds(new_alt);
        }
        return node;
    } else 
    {
        std::cout << "Node " << node->name << " not found.";
        return node;
    }
}

// Image reconstruction
cv::Mat Tree_of_shapes::reconstruct_image()
{
    cv::Mat im = cv::Mat::zeros(image.size(), CV_8UC1);

    if (root == nullptr)
    {
        return im;
    }

    std::queue<const Node_tos*> q;
    q.push(root);

    while (!q.empty())
    {
        const Node_tos* node = q.front();
        q.pop();

        for (long px : node->proper_part)
        {
            int row = static_cast<int>(px / image.cols);
            int col = static_cast<int>(px % image.cols);

            im.at<uchar>(row, col) = static_cast<uchar>(node->alt);
        }

        for (const Node_tos* child : node->children)
        {
            if(child != nullptr)
            {
                q.push(child);
            }
        }
    }

    return im;
}


cv::Vec3b color_from_id(long id)
{
    uchar b = static_cast<uchar>(50 + (id * 53) % 206);
    uchar g = static_cast<uchar>(50 + (id * 97) % 206);
    uchar r = static_cast<uchar>(50 + (id * 193) % 206);

    return cv::Vec3b(b, g, r);
}

cv::Mat Tree_of_shapes::reconstruct_node_colored_image()
{
    cv::Mat im = cv::Mat::zeros(image.size(), CV_8UC3);

    if (root == nullptr)
    {
        return im;
    }

    std::queue<const Node_tos*> q;
    q.push(root);

    while (!q.empty())
    {
        const Node_tos* node = q.front();
        q.pop();

        cv::Vec3b color = color_from_id(node->name);

        for (long px : node->proper_part)
        {
            int row = static_cast<int>(px / image.cols);
            int col = static_cast<int>(px % image.cols);

            im.at<cv::Vec3b>(row, col) = color;
        }

        for (const Node_tos* child : node->children)
        {
            if (child != nullptr)
            {
                q.push(child);
            }
        }
    }

    return im;
}

// Debugging
void Tree_of_shapes::traverse_tree()
{
    std::vector<long> stack = {tos.tree.root() - img_size};

    while (!stack.empty())
    {
        long current_id = stack.back();
        stack.pop_back();

        Node_tos *current = nodes[current_id].get();
        if (current->parent != nullptr)
        {
            std::cout << "Node " << current_id << ", alt " << current->alt << ", parent " << current->parent->name << "\n";
        } else
        {
            std::cout << "Node " << current_id << ", alt " << current->alt << "\n";
        }

        for (auto child : current->children)
        {
            stack.push_back(child->name);
        }
    }
}

// Debugging
void Tree_of_shapes::print_parents() const
{
    std::cout << "Tree of Shapes -- parents:\n";
    for (size_t i = img_size; i < parents.size(); ++i)
    {
        std::cout << i - img_size << "(" << (int)tos.altitudes[i] << ") -> " << parents[i] - img_size << "\n";
    }
}

long Tree_of_shapes::nb_nodes()
{
    int count = 0;
    for (const auto& [name, node] : nodes)
    {
        if (!node.get()->removed)
        {
            count++;
        }
    }
    return count;
}