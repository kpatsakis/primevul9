setup_tree(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      Node* prev = NULL_NODE;
      do {
        r = setup_tree(NODE_CAR(node), reg, state, env);
        if (IS_NOT_NULL(prev) && r == 0) {
          r = next_setup(prev, NODE_CAR(node), reg);
        }
        prev = NODE_CAR(node);
      } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_ALT:
    do {
      r = setup_tree(NODE_CAR(node), reg, (state | IN_ALT), env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_STRING:
    if (IS_IGNORECASE(reg->options) && !NODE_STRING_IS_RAW(node)) {
      r = expand_case_fold_string(node, reg);
    }
    break;

  case NODE_BACKREF:
    {
      int i;
      int* p;
      BackRefNode* br = BACKREF_(node);
      p = BACKREFS_P(br);
      for (i = 0; i < br->back_num; i++) {
        if (p[i] > env->num_mem)  return ONIGERR_INVALID_BACKREF;
        MEM_STATUS_ON(env->backrefed_mem, p[i]);
        MEM_STATUS_ON(env->bt_mem_start, p[i]);
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          MEM_STATUS_ON(env->bt_mem_end, p[i]);
        }
#endif
      }
    }
    break;

  case NODE_ENCLOSURE:
    {
      EnclosureNode* en = ENCLOSURE_(node);

      switch (en->type) {
      case ENCLOSURE_OPTION:
        {
          OnigOptionType options = reg->options;
          reg->options = ENCLOSURE_(node)->o.options;
          r = setup_tree(NODE_BODY(node), reg, state, env);
          reg->options = options;
        }
        break;

      case ENCLOSURE_MEMORY:
#ifdef USE_CALL
        state |= en->m.called_state;
#endif

        if ((state & (IN_ALT | IN_NOT | IN_VAR_REPEAT | IN_MULTI_ENTRY)) != 0
            || NODE_IS_RECURSION(node)) {
          MEM_STATUS_ON(env->bt_mem_start, en->m.regnum);
        }
        r = setup_tree(NODE_BODY(node), reg, state, env);
        break;

      case ENCLOSURE_STOP_BACKTRACK:
        {
          Node* target = NODE_BODY(node);
          r = setup_tree(target, reg, state, env);
          if (NODE_TYPE(target) == NODE_QUANT) {
            QuantNode* tqn = QUANT_(target);
            if (IS_REPEAT_INFINITE(tqn->upper) && tqn->lower <= 1 &&
                tqn->greedy != 0) {  /* (?>a*), a*+ etc... */
              if (NODE_IS_SIMPLE_TYPE(NODE_BODY(target)))
                NODE_STATUS_ADD(node, NST_STOP_BT_SIMPLE_REPEAT);
            }
          }
        }
        break;

      case ENCLOSURE_IF_ELSE:
        r = setup_tree(NODE_BODY(node), reg, (state | IN_ALT), env);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = setup_tree(en->te.Then, reg, (state | IN_ALT), env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = setup_tree(en->te.Else, reg, (state | IN_ALT), env);
        break;
      }
    }
    break;

  case NODE_QUANT:
    r = setup_quant(node, reg, state, env);
    break;

  case NODE_ANCHOR:
    r = setup_anchor(node, reg, state, env);
    break;

#ifdef USE_CALL
  case NODE_CALL:
#endif
  case NODE_CTYPE:
  case NODE_CCLASS:
  case NODE_GIMMICK:
  default:
    break;
  }

  return r;
}