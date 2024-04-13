renumber_backref_traverse(Node* node, GroupNumMap* map)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = renumber_backref_traverse(NODE_CAR(node), map);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    r = renumber_backref_traverse(NODE_BODY(node), map);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      r = renumber_backref_traverse(NODE_BODY(node), map);
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = renumber_backref_traverse(en->te.Then, map);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = renumber_backref_traverse(en->te.Else, map);
          if (r != 0) return r;
        }
      }
    }
    break;

  case NODE_BACKREF:
    r = renumber_backref_node(node, map);
    break;

  case NODE_ANCHOR:
    if (IS_NOT_NULL(NODE_BODY(node)))
      r = renumber_backref_traverse(NODE_BODY(node), map);
    break;

  default:
    break;
  }

  return r;
}