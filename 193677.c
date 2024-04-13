setup_call2_call(Node* node)
{
  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      setup_call2_call(NODE_CAR(node));
    } while (IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    setup_call2_call(NODE_BODY(node));
    break;

  case NODE_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      setup_call2_call(NODE_BODY(node));
    break;

  case NODE_ENCLOSURE:
    {
      EnclosureNode* en = ENCLOSURE_(node);

      if (en->type == ENCLOSURE_MEMORY) {
        if (! NODE_IS_MARK1(node)) {
          NODE_STATUS_ADD(node, NST_MARK1);
          setup_call2_call(NODE_BODY(node));
          NODE_STATUS_REMOVE(node, NST_MARK1);
        }
      }
      else if (en->type == ENCLOSURE_IF_ELSE) {
        setup_call2_call(NODE_BODY(node));
        if (IS_NOT_NULL(en->te.Then))
          setup_call2_call(en->te.Then);
        if (IS_NOT_NULL(en->te.Else))
          setup_call2_call(en->te.Else);
      }
      else {
        setup_call2_call(NODE_BODY(node));
      }
    }
    break;

  case NODE_CALL:
    if (! NODE_IS_MARK1(node)) {
      NODE_STATUS_ADD(node, NST_MARK1);
      {
        CallNode* cn = CALL_(node);
        Node* called = NODE_CALL_BODY(cn);

        cn->entry_count++;

        NODE_STATUS_ADD(called, NST_CALLED);
        ENCLOSURE_(called)->m.entry_count++;
        setup_call2_call(called);
      }
      NODE_STATUS_REMOVE(node, NST_MARK1);
    }
    break;

  default:
    break;
  }
}