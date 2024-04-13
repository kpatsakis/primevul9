setup_quant(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int r;
  OnigLen d;
  QuantNode* qn = QUANT_(node);
  Node* body = NODE_BODY(node);

  if ((state & IN_REAL_REPEAT) != 0) {
    NODE_STATUS_ADD(node, NST_IN_REAL_REPEAT);
  }
  if ((state & IN_MULTI_ENTRY) != 0) {
    NODE_STATUS_ADD(node, NST_IN_MULTI_ENTRY);
  }

  if (IS_REPEAT_INFINITE(qn->upper) || qn->upper >= 1) {
    d = tree_min_len(body, env);
    if (d == 0) {
#ifdef USE_INSISTENT_CHECK_CAPTURES_IN_EMPTY_REPEAT
      qn->body_empty_info = quantifiers_memory_node_info(body);
      if (qn->body_empty_info == QUANT_BODY_IS_EMPTY_REC) {
        if (NODE_TYPE(body) == NODE_ENCLOSURE &&
            ENCLOSURE_(body)->type == ENCLOSURE_MEMORY) {
          MEM_STATUS_ON(env->bt_mem_end, ENCLOSURE_(body)->m.regnum);
        }
      }
#else
      qn->body_empty_info = QUANT_BODY_IS_EMPTY;
#endif
    }
  }

  if (IS_REPEAT_INFINITE(qn->upper) || qn->upper >= 2)
    state |= IN_REAL_REPEAT;
  if (qn->lower != qn->upper)
    state |= IN_VAR_REPEAT;

  r = setup_tree(body, reg, state, env);
  if (r != 0) return r;

  /* expand string */
#define EXPAND_STRING_MAX_LENGTH  100
  if (NODE_TYPE(body) == NODE_STRING) {
    if (!IS_REPEAT_INFINITE(qn->lower) && qn->lower == qn->upper &&
        qn->lower > 1 && qn->lower <= EXPAND_STRING_MAX_LENGTH) {
      int len = NODE_STRING_LEN(body);
      StrNode* sn = STR_(body);

      if (len * qn->lower <= EXPAND_STRING_MAX_LENGTH) {
        int i, n = qn->lower;
        onig_node_conv_to_str_node(node, STR_(body)->flag);
        for (i = 0; i < n; i++) {
          r = onig_node_str_cat(node, sn->s, sn->end);
          if (r != 0) return r;
        }
        onig_node_free(body);
        return r;
      }
    }
  }

#ifdef USE_OP_PUSH_OR_JUMP_EXACT
  if (qn->greedy && (qn->body_empty_info != QUANT_BODY_IS_NOT_EMPTY)) {
    if (NODE_TYPE(body) == NODE_QUANT) {
      QuantNode* tqn = QUANT_(body);
      if (IS_NOT_NULL(tqn->head_exact)) {
        qn->head_exact  = tqn->head_exact;
        tqn->head_exact = NULL;
      }
    }
    else {
      qn->head_exact = get_head_value_node(NODE_BODY(node), 1, reg);
    }
  }
#endif

  return r;
}