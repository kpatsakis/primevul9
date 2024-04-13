is_ancestor_node(Node* node, Node* me)
{
  Node* parent;

  while ((parent = NODE_PARENT(me)) != NULL_NODE) {
    if (parent == node) return 1;
    me = parent;
  }
  return 0;
}