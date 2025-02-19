check_node_in_look_behind(Node* node, int not, int* used)
{
  static unsigned int
    bag_mask[2] = { ALLOWED_BAG_IN_LB, ALLOWED_BAG_IN_LB_NOT };

  static unsigned int
    anchor_mask[2] = { ALLOWED_ANCHOR_IN_LB, ALLOWED_ANCHOR_IN_LB_NOT };

  NodeType type;
  int r = 0;

  type = NODE_TYPE(node);
  if ((NODE_TYPE2BIT(type) & ALLOWED_TYPE_IN_LB) == 0)
    return 1;

  switch (type) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = check_node_in_look_behind(NODE_CAR(node), not, used);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    r = check_node_in_look_behind(NODE_BODY(node), not, used);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      if (((1<<en->type) & bag_mask[not]) == 0)
        return 1;

      r = check_node_in_look_behind(NODE_BODY(node), not, used);
      if (r != 0) break;

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_BACKREF(node) || NODE_IS_CALLED(node)
         || NODE_IS_REFERENCED(node))
          *used = TRUE;
      }
      else if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = check_node_in_look_behind(en->te.Then, not, used);
          if (r != 0) break;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = check_node_in_look_behind(en->te.Else, not, used);
        }
      }
    }
    break;

  case NODE_ANCHOR:
    type = ANCHOR_(node)->type;
    if ((type & anchor_mask[not]) == 0)
      return 1;

    if (IS_NOT_NULL(NODE_BODY(node)))
      r = check_node_in_look_behind(NODE_BODY(node), not, used);
    break;

  case NODE_GIMMICK:
    if (NODE_IS_ABSENT_WITH_SIDE_EFFECTS(node) != 0)
      return 1;
    break;

  case NODE_CALL:
    r = check_called_node_in_look_behind(NODE_BODY(node), not);
    break;

  default:
    break;
  }
  return r;
}