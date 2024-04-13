check_called_node_in_look_behind(Node* node, int not)
{
  int r;

  r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = check_called_node_in_look_behind(NODE_CAR(node), not);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    r = check_called_node_in_look_behind(NODE_BODY(node), not);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_MARK1(node))
          return 0;
        else {
          NODE_STATUS_ADD(node, MARK1);
          r = check_called_node_in_look_behind(NODE_BODY(node), not);
          NODE_STATUS_REMOVE(node, MARK1);
        }
      }
      else {
        r = check_called_node_in_look_behind(NODE_BODY(node), not);
        if (r == 0 && en->type == BAG_IF_ELSE) {
          if (IS_NOT_NULL(en->te.Then)) {
            r = check_called_node_in_look_behind(en->te.Then, not);
            if (r != 0) break;
          }
          if (IS_NOT_NULL(en->te.Else)) {
            r = check_called_node_in_look_behind(en->te.Else, not);
          }
        }
      }
    }
    break;

  case NODE_ANCHOR:
    if (IS_NOT_NULL(NODE_BODY(node)))
      r = check_called_node_in_look_behind(NODE_BODY(node), not);
    break;

  case NODE_GIMMICK:
    if (NODE_IS_ABSENT_WITH_SIDE_EFFECTS(node) != 0)
      return 1;
    break;

  default:
    break;
  }

  return r;
}