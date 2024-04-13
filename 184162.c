quantifiers_memory_node_info(Node* node)
{
  int r = BODY_MAY_BE_EMPTY;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    {
      int v;
      do {
        v = quantifiers_memory_node_info(NODE_CAR(node));
        if (v > r) r = v;
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    if (NODE_IS_RECURSION(node)) {
      return BODY_MAY_BE_EMPTY_REC; /* tiny version */
    }
    else
      r = quantifiers_memory_node_info(NODE_BODY(node));
    break;
#endif

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->upper != 0) {
        r = quantifiers_memory_node_info(NODE_BODY(node));
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_MEMORY:
        if (NODE_IS_RECURSION(node)) {
          return BODY_MAY_BE_EMPTY_REC;
        }
        return BODY_MAY_BE_EMPTY_MEM;
        break;

      case BAG_OPTION:
      case BAG_STOP_BACKTRACK:
        r = quantifiers_memory_node_info(NODE_BODY(node));
        break;
      case BAG_IF_ELSE:
        {
          int v;
          r = quantifiers_memory_node_info(NODE_BODY(node));
          if (IS_NOT_NULL(en->te.Then)) {
            v = quantifiers_memory_node_info(en->te.Then);
            if (v > r) r = v;
          }
          if (IS_NOT_NULL(en->te.Else)) {
            v = quantifiers_memory_node_info(en->te.Else);
            if (v > r) r = v;
          }
        }
        break;
      }
    }
    break;

  case NODE_BACKREF:
  case NODE_STRING:
  case NODE_CTYPE:
  case NODE_CCLASS:
  case NODE_ANCHOR:
  case NODE_GIMMICK:
  default:
    break;
  }

  return r;
}