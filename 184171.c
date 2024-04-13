infinite_recursive_call_check(Node* node, ScanEnv* env, int head)
{
  int ret;
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      Node *x;
      OnigLen min;

      x = node;
      do {
        ret = infinite_recursive_call_check(NODE_CAR(x), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;
        r |= ret;
        if (head != 0) {
          min = node_min_byte_len(NODE_CAR(x), env);
          if (min != 0) head = 0;
        }
      } while (IS_NOT_NULL(x = NODE_CDR(x)));
    }
    break;

  case NODE_ALT:
    {
      int must;

      must = RECURSION_MUST;
      do {
        ret = infinite_recursive_call_check(NODE_CAR(node), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;

        r    |= (ret & RECURSION_EXIST);
        must &= ret;
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
      r |= must;
    }
    break;

  case NODE_QUANT:
    r = infinite_recursive_call_check(NODE_BODY(node), env, head);
    if (r < 0) return r;
    if ((r & RECURSION_MUST) != 0) {
      if (QUANT_(node)->lower == 0)
        r &= ~RECURSION_MUST;
    }
    break;

  case NODE_ANCHOR:
    if (! ANCHOR_HAS_BODY(ANCHOR_(node)))
      break;
    /* fall */
  case NODE_CALL:
    r = infinite_recursive_call_check(NODE_BODY(node), env, head);
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      if (en->type == BAG_MEMORY) {
        if (NODE_IS_MARK2(node))
          return 0;
        else if (NODE_IS_MARK1(node))
          return (head == 0 ? RECURSION_EXIST | RECURSION_MUST
                  : RECURSION_EXIST | RECURSION_MUST | RECURSION_INFINITE);
        else {
          NODE_STATUS_ADD(node, MARK2);
          r = infinite_recursive_call_check(NODE_BODY(node), env, head);
          NODE_STATUS_REMOVE(node, MARK2);
        }
      }
      else if (en->type == BAG_IF_ELSE) {
        int eret;

        ret = infinite_recursive_call_check(NODE_BODY(node), env, head);
        if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;
        r |= ret;
        if (IS_NOT_NULL(en->te.Then)) {
          OnigLen min;
          if (head != 0) {
            min = node_min_byte_len(NODE_BODY(node), env);
          }
          else min = 0;

          ret = infinite_recursive_call_check(en->te.Then, env, min != 0 ? 0:head);
          if (ret < 0 || (ret & RECURSION_INFINITE) != 0) return ret;
          r |= ret;
        }
        if (IS_NOT_NULL(en->te.Else)) {
          eret = infinite_recursive_call_check(en->te.Else, env, head);
          if (eret < 0 || (eret & RECURSION_INFINITE) != 0) return eret;
          r |= (eret & RECURSION_EXIST);
          if ((eret & RECURSION_MUST) == 0)
            r &= ~RECURSION_MUST;
        }
        else {
          r &= ~RECURSION_MUST;
        }
      }
      else {
        r = infinite_recursive_call_check(NODE_BODY(node), env, head);
      }
    }
    break;

  default:
    break;
  }

  return r;
}