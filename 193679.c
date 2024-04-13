setup_call(Node* node, ScanEnv* env, int state)
{
  int r;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
  case NODE_ALT:
    do {
      r = setup_call(NODE_CAR(node), env, state);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_QUANT:
    if (QUANT_(node)->upper == 0)
      state |= IN_ZERO_REPEAT;

    r = setup_call(NODE_BODY(node), env, state);
    break;

  case NODE_ANCHOR:
    if (ANCHOR_HAS_BODY(ANCHOR_(node)))
      r = setup_call(NODE_BODY(node), env, state);
    else
      r = 0;
    break;

  case NODE_ENCLOSURE:
    {
      EnclosureNode* en = ENCLOSURE_(node);

      if (en->type == ENCLOSURE_MEMORY) {
        if ((state & IN_ZERO_REPEAT) != 0) {
          NODE_STATUS_ADD(node, NST_IN_ZERO_REPEAT);
          ENCLOSURE_(node)->m.entry_count--;
        }
        r = setup_call(NODE_BODY(node), env, state);
      }
      else if (en->type == ENCLOSURE_IF_ELSE) {
        r = setup_call(NODE_BODY(node), env, state);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = setup_call(en->te.Then, env, state);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = setup_call(en->te.Else, env, state);
      }
      else
        r = setup_call(NODE_BODY(node), env, state);
    }
    break;

  case NODE_CALL:
    if ((state & IN_ZERO_REPEAT) != 0) {
      NODE_STATUS_ADD(node, NST_IN_ZERO_REPEAT);
      CALL_(node)->entry_count--;
    }

    r = setup_call_node_call(CALL_(node), env, state);
    break;

  default:
    r = 0;
    break;
  }

  return r;
}