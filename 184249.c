recursive_call_check(Node* node)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    r = 0;
    do {
      r |= recursive_call_check(NODE_CAR(node));
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case NODE_QUANT:
    r = recursive_call_check(NODE_BODY(node));
    break;

  case NODE_CALL:
    r = recursive_call_check(NODE_BODY(node));
    if (r != 0) {
      if (NODE_IS_MARK1(NODE_BODY(node)))
        NODE_STATUS_ADD(node, RECURSION);
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_MARK2(node))
          return 0;
        else if (NODE_IS_MARK1(node))
          return 1; /* recursion */
        else {
          NODE_STATUS_ADD(node, MARK2);
          r = recursive_call_check(NODE_BODY(node));
          NODE_STATUS_REMOVE(node, MARK2);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        r = 0;
        if (IS_NOT_NULL(en->te.Then)) {
          r |= recursive_call_check(en->te.Then);
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r |= recursive_call_check(en->te.Else);
        }
        r |= recursive_call_check(NODE_BODY(node));
      }
      else {
        r = recursive_call_check(NODE_BODY(node));
      }
    }
    break;

  default:
    r = 0;
    break;
  }

  return r;
}