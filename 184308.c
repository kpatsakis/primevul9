get_tree_tail_literal(Node* node, Node** rnode, regex_t* reg)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    if (IS_NULL(NODE_CDR(node))) {
      r = get_tree_tail_literal(NODE_CAR(node), rnode, reg);
    }
    else {
      r = get_tree_tail_literal(NODE_CDR(node), rnode, reg);
      if (r == GET_VALUE_IGNORE) {
        r = get_tree_tail_literal(NODE_CAR(node), rnode, reg);
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    r = get_tree_tail_literal(NODE_BODY(node), rnode, reg);
    break;
#endif

  case NODE_CTYPE:
    if (CTYPE_(node)->ctype == CTYPE_ANYCHAR) {
      r = GET_VALUE_NONE;
      break;
    }
    /* fall */
  case NODE_CCLASS:
    *rnode = node;
    r = GET_VALUE_FOUND;
    break;

  case NODE_STRING:
    {
      StrNode* sn = STR_(node);

      if (sn->end <= sn->s) {
        r = GET_VALUE_IGNORE;
        break;
      }

      if (NODE_IS_IGNORECASE(node) && ! NODE_STRING_IS_CRUDE(node)) {
        r = GET_VALUE_NONE;
        break;
      }

      *rnode = node;
      r = GET_VALUE_FOUND;
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);
      if (qn->lower != 0) {
        r = get_tree_tail_literal(NODE_BODY(node), rnode, reg);
      }
      else
        r = GET_VALUE_NONE;
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_MARK1(node))
          r = GET_VALUE_NONE;
        else {
          NODE_STATUS_ADD(node, MARK1);
          r = get_tree_tail_literal(NODE_BODY(node), rnode, reg);
          NODE_STATUS_REMOVE(node, MARK1);
        }
      }
      else {
        r = get_tree_tail_literal(NODE_BODY(node), rnode, reg);
      }
    }
    break;

  case NODE_ANCHOR:
  case NODE_GIMMICK:
    r = GET_VALUE_IGNORE;
    break;

  case NODE_ALT:
  case NODE_BACKREF:
  default:
    r = GET_VALUE_NONE;
    break;
  }

  return r;
}