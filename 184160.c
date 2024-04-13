infinite_recursive_call_check_trav(Node* node, ScanEnv* env)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = infinite_recursive_call_check_trav(NODE_CAR(node), env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node))) {
      r = 0;
      break;
    }
    /* fall */
  case NODE_QUANT:
    r = infinite_recursive_call_check_trav(NODE_BODY(node), env);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_RECURSION(node) && NODE_IS_CALLED(node)) {
          int ret;

          NODE_STATUS_ADD(node, MARK1);

          ret = infinite_recursive_call_check(NODE_BODY(node), env, 1);
          if (ret < 0) return ret;
          else if ((ret & (RECURSION_MUST | RECURSION_INFINITE)) != 0)
            return ONIGERR_NEVER_ENDING_RECURSION;

          NODE_STATUS_REMOVE(node, MARK1);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        if (IS_NOT_NULL(en->te.Then)) {
          r = infinite_recursive_call_check_trav(en->te.Then, env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          r = infinite_recursive_call_check_trav(en->te.Else, env);
          if (r != 0) return r;
        }
      }
    }

    r = infinite_recursive_call_check_trav(NODE_BODY(node), env);
    break;

  default:
    r = 0;
    break;
  }

  return r;
}