get_tree_head_literal(Node* node, int exact, regex_t* reg)
{
  Node* n = NULL_NODE;

  switch (NODE_TYPE(node)) {
  case NODE_BACKREF:
  case NODE_ALT:
#ifdef USE_CALL
  case NODE_CALL:
#endif
    break;

  case NODE_CTYPE:
    if (CTYPE_(node)->ctype == CTYPE_ANYCHAR)
      break;
    /* fall */
  case NODE_CCLASS:
    if (exact == 0) {
      n = node;
    }
    break;

  case NODE_LIST:
    n = get_tree_head_literal(NODE_CAR(node), exact, reg);
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);

      if (sn->end <= sn->s)
        break;

      if (exact == 0 ||
          ! NODE_IS_IGNORECASE(node) || NODE_STRING_IS_CRUDE(node)) {
        n = node;
      }
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->lower > 0) {
        if (IS_NOT_NULL(qn->head_exact))
          n = qn->head_exact;
        else
          n = get_tree_head_literal(NODE_BODY(node), exact, reg);
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);
      switch (en->type) {
      case BAG_OPTION:
      case BAG_MEMORY:
      case BAG_STOP_BACKTRACK:
      case BAG_IF_ELSE:
        n = get_tree_head_literal(NODE_BODY(node), exact, reg);
        break;
      }
    }
    break;

  case NODE_ANCHOR:
    if (ANCHOR_(node)->type == ANCR_PREC_READ)
      n = get_tree_head_literal(NODE_BODY(node), exact, reg);
    break;

  case NODE_GIMMICK:
  default:
    break;
  }

  return n;
}