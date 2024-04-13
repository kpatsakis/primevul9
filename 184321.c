set_parent_node_trav(Node* node, Node* parent)
{
  NODE_PARENT(node) = parent;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      set_parent_node_trav(NODE_CAR(node), node);
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) break;
    set_parent_node_trav(NODE_BODY(node), node);
    break;

  case NODE_QUANT:
    set_parent_node_trav(NODE_BODY(node), node);
    break;

  case NODE_BAG:
    if (IS_NOT_NULL(NODE_BODY(node)))
      set_parent_node_trav(NODE_BODY(node), node);
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then))
          set_parent_node_trav(en->te.Then, node);
        if (IS_NOT_NULL(en->te.Else)) {
          set_parent_node_trav(en->te.Else, node);
        }
      }
    }
    break;

  default:
    break;
  }
}