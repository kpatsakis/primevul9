tune_call2_call(Node* node)
{
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      tune_call2_call(NODE_CAR(node));
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    tune_call2_call(NODE_BODY(node));
    break;

  case NODE_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      tune_call2_call(NODE_BODY(node));
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (! NODE_IS_MARK1(node)) {
          NODE_STATUS_ADD(node, MARK1);
          tune_call2_call(NODE_BODY(node));
          NODE_STATUS_REMOVE(node, MARK1);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        tune_call2_call(NODE_BODY(node));
        if (IS_NOT_NULL(en->te.Then))
          tune_call2_call(en->te.Then);
        if (IS_NOT_NULL(en->te.Else))
          tune_call2_call(en->te.Else);
      }
      else {
        tune_call2_call(NODE_BODY(node));
      }
    }
    break;

  case NODE_CALL:
    if (! NODE_IS_MARK1(node)) {
      NODE_STATUS_ADD(node, MARK1);
      {
        CallNode* cn = CALL_(node);
        Node* called = NODE_CALL_BODY(cn);

        cn->entry_count++;

        NODE_STATUS_ADD(called, CALLED);
        BAG_(called)->m.entry_count++;
        tune_call2_call(called);
      }
      NODE_STATUS_REMOVE(node, MARK1);
    }
    break;

  default:
    break;
  }
}