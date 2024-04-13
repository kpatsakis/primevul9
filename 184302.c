numbered_ref_check(Node* node)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = numbered_ref_check(NODE_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (IS_NULL(NODE_BODY(node)))
      break;
    /* fall */
  case NODE_QUANT:
    r = numbered_ref_check(NODE_BODY(node));
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      r = numbered_ref_check(NODE_BODY(node));
      if (r != 0) return r;

      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = numbered_ref_check(en->te.Then);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = numbered_ref_check(en->te.Else);
          if (r != 0) return r;
        }
      }
    }

    break;

  case NODE_BACKREF:
    if (! NODE_IS_BY_NAME(node))
      return ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED;
    break;

  default:
    break;
  }

  return r;
}