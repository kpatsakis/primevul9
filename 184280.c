tune_tree(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    {
      Node* prev = NULL_NODE;
      do {
        r = tune_tree(NODE_CAR(node), reg, state, env);
        if (IS_NOT_NULL(prev) && r == 0) {
          r = tune_next(prev, NODE_CAR(node), reg);
        }
        prev = NODE_CAR(node);
      } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_ALT:
    do {
      r = tune_tree(NODE_CAR(node), reg, (state | IN_ALT), env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_STRING:
    if (NODE_IS_IGNORECASE(node) && ! NODE_STRING_IS_CRUDE(node)) {
      r = unravel_case_fold_string(node, reg, state);
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
#if 0
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          MEM_STATUS_ON(env->backtrack_mem, p[i]);
        }
#endif
#else
        /* More precisely, it should be checked whether alt/repeat exists before
           the subject capture node, and then this backreference position
           exists before (or in) the capture node. */
        MEM_STATUS_ON(env->backtrack_mem, p[i]);
#endif
      }
    }
    break;

  case NODE_BAG:
    {
      BagNode* en = BAG_(node);

      switch (en->type) {
      case BAG_OPTION:
        {
          OnigOptionType options = reg->options;
          reg->options = BAG_(node)->o.options;
          r = tune_tree(NODE_BODY(node), reg, state, env);
          reg->options = options;
        }
        break;

      case BAG_MEMORY:
#ifdef USE_CALL
        state |= en->m.called_state;
#endif

        if ((state & (IN_ALT | IN_NOT | IN_VAR_REPEAT | IN_MULTI_ENTRY)) != 0
            || NODE_IS_RECURSION(node)) {
          MEM_STATUS_ON(env->backtrack_mem, en->m.regnum);
        }
        r = tune_tree(NODE_BODY(node), reg, state, env);
        break;

      case BAG_STOP_BACKTRACK:
        {
          Node* target = NODE_BODY(node);
          r = tune_tree(target, reg, state, env);
          if (NODE_TYPE(target) == NODE_QUANT) {
            QuantNode* tqn = QUANT_(target);
            if (IS_INFINITE_REPEAT(tqn->upper) && tqn->lower <= 1 &&
                tqn->greedy != 0) {  /* (?>a*), a*+ etc... */
              if (is_strict_real_node(NODE_BODY(target)))
                NODE_STATUS_ADD(node, STRICT_REAL_REPEAT);
            }
          }
        }
        break;

      case BAG_IF_ELSE:
        r = tune_tree(NODE_BODY(node), reg, (state | IN_ALT), env);
        if (r != 0) return r;
        if (IS_NOT_NULL(en->te.Then)) {
          r = tune_tree(en->te.Then, reg, (state | IN_ALT), env);
          if (r != 0) return r;
        }
        if (IS_NOT_NULL(en->te.Else))
          r = tune_tree(en->te.Else, reg, (state | IN_ALT), env);
        break;
      }
    }
    break;

  case NODE_QUANT:
    r = tune_quant(node, reg, state, env);
    break;

  case NODE_ANCHOR:
    r = tune_anchor(node, reg, state, env);
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