recursive_call_check_trav(Node* node, ScanEnv* env, int state)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    {
      int ret;
      do {
        ret = recursive_call_check_trav(NODE_CAR(node), env, state);
        if (ret == FOUND_CALLED_NODE) r = FOUND_CALLED_NODE;
        else if (ret < 0) return ret;
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_QUANT:
    r = recursive_call_check_trav(NODE_BODY(node), env, state);
    if (QUANT_(node)->upper == 0) {
      if (r == FOUND_CALLED_NODE)
        QUANT_(node)->is_refered = 1;
    }
    break;

  case NODE_ANCHOR:
    {
      AnchorNode* an = ANCHOR_(node);
      if (ANCHOR_HAS_BODY(an))
        r = recursive_call_check_trav(NODE_ANCHOR_BODY(an), env, state);
    }
    break;

  case NODE_ENCLOSURE:
    {
      int ret;
      int state1;
      EnclosureNode* en = ENCLOSURE_(node);

      if (en->type == ENCLOSURE_MEMORY) {
        if (NODE_IS_CALLED(node) || (state & IN_RECURSION) != 0) {
          if (! NODE_IS_RECURSION(node)) {
            NODE_STATUS_ADD(node, NST_MARK1);
            r = recursive_call_check(NODE_BODY(node));
            if (r != 0)
              NODE_STATUS_ADD(node, NST_RECURSION);
            NODE_STATUS_REMOVE(node, NST_MARK1);
          }

          if (NODE_IS_CALLED(node))
            r = FOUND_CALLED_NODE;
        }
      }

      state1 = state;
      if (NODE_IS_RECURSION(node))
        state1 |= IN_RECURSION;

      ret = recursive_call_check_trav(NODE_BODY(node), env, state1);
      if (ret == FOUND_CALLED_NODE)
        r = FOUND_CALLED_NODE;

      if (en->type == ENCLOSURE_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          ret = recursive_call_check_trav(en->te.Then, env, state1);
          if (ret == FOUND_CALLED_NODE)
            r = FOUND_CALLED_NODE;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          ret = recursive_call_check_trav(en->te.Else, env, state1);
          if (ret == FOUND_CALLED_NODE)
            r = FOUND_CALLED_NODE;
        }
      }
    }
    break;

  default:
    break;
  }

  return r;
}