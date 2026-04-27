#include "node_ct.h"

Node_ct::Node_ct(int id, int alt, Node_class node_class, Node_ct *parent)
    : name(id), alt(alt), node_class(node_class), parent(parent)
{
    if (parent != nullptr)
    {
        parent->children.push_back(this);
    }
}
