set_empty_repeat_node_trav(Node* node, Node* empty, ScanEnv* env)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = set_empty_repeat_node_trav(NODE_CAR(node), empty, env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);

      if (! ANCHOR_HAS_BODY(an)) {
        r = 0;
        break;
      }

      switch (an->type) {
      case ANCR_PREC_READ:
      case ANCR_LOOK_BEHIND:
        empty = NULL_NODE;
        break;
      default:
        break;
      }
      r = set_empty_repeat_node_trav(NODE_BODY(node), empty, env);
    }
    break;

  case NODE_QUANT:
    {
      QuantNode* qn = QUANT_(node);

      if (qn->emptiness != BODY_IS_NOT_EMPTY) empty = node;
      r = set_empty_repeat_node_trav(NODE_BODY(node), empty, env);
    }
    break;

  case NODE_BAG:
    if (IS_NOT_NULL(NODE_BODY(node))) {
      r = set_empty_repeat_node_trav(NODE_BODY(node), empty, env);
      if (r != 0) return r;
    }
    {
      BagNode* en = BAG_(node);

      r = 0;
      if (en->type == BAG_MEMORY) {
        if (NODE_IS_BACKREF(node)) {
          if (IS_NOT_NULL(empty))
            SCANENV_MEMENV(env)[en->m.regnum].empty_repeat_node = empty;
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = set_empty_repeat_node_trav(en->te.Then, empty, env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = set_empty_repeat_node_trav(en->te.Else, empty, env);
        }
      }
    }
    break;

  default:
    r = 0;
    break;
  }

  return r;
}