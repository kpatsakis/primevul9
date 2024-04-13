tune_call2(Node* node)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = tune_call2(NODE_CAR(node));
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    if (QUANT_(node)->upper != 0)
      r = tune_call2(NODE_BODY(node));
    break;

  case NODE_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = tune_call2(NODE_BODY(node));
    break;

  case NODE_BAG:
    if (! NODE_IS_IN_ZERO_REPEAT(node))
      r = tune_call2(NODE_BODY(node));

    {
      BagNode* en = BAG_(node);

      if (r != 0) return r;
      if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = tune_call2(en->te.Then);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = tune_call2(en->te.Else);
      }
    }
    break;

  case NODE_CALL:
    if (! NODE_IS_IN_ZERO_REPEAT(node)) {
      tune_call2_call(node);
    }
    break;

  default:
    break;
  }

  return r;
}