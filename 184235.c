tune_quant(Node* node, regex_t* reg, int state, ScanEnv* env)
{
  int r;
  QuantNode* qn = QUANT_(node);
  Node* body = NODE_BODY(node);

  if ((state & IN_REAL_REPEAT) != 0) {
    NODE_STATUS_ADD(node, IN_REAL_REPEAT);
  }
  if ((state & IN_MULTI_ENTRY) != 0) {
    NODE_STATUS_ADD(node, IN_MULTI_ENTRY);
  }

  if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 1) {
    OnigLen d = node_min_byte_len(body, env);
    if (d == 0) {
#ifdef USE_STUBBORN_CHECK_CAPTURES_IN_EMPTY_REPEAT
      qn->emptiness = quantifiers_memory_node_info(body);
#else
      qn->emptiness = BODY_MAY_BE_EMPTY;
#endif
    }
  }

  if (IS_INFINITE_REPEAT(qn->upper) || qn->upper >= 2)
    state |= IN_REAL_REPEAT;
  if (qn->lower != qn->upper)
    state |= IN_VAR_REPEAT;

  r = tune_tree(body, reg, state, env);
  if (r != 0) return r;

  /* expand string */
#define EXPAND_STRING_MAX_LENGTH  100
  if (NODE_TYPE(body) == NODE_STRING) {
    if (!IS_INFINITE_REPEAT(qn->lower) && qn->lower == qn->upper &&
        qn->lower > 1 && qn->lower <= EXPAND_STRING_MAX_LENGTH) {
      int len = NODE_STRING_LEN(body);

      if (len * qn->lower <= EXPAND_STRING_MAX_LENGTH) {
        int i, n = qn->lower;
        node_conv_to_str_node(node, body);
        for (i = 0; i < n; i++) {
          r = node_str_node_cat(node, body);
          if (r != 0) return r;
        }
        onig_node_free(body);
        return r;
      }
    }
  }

  if (qn->greedy && (qn->emptiness == BODY_IS_NOT_EMPTY)) {
    if (NODE_TYPE(body) == NODE_QUANT) {
      QuantNode* tqn = QUANT_(body);
      if (IS_NOT_NULL(tqn->head_exact)) {
        qn->head_exact  = tqn->head_exact;
        tqn->head_exact = NULL;
      }
    }
    else {
      qn->head_exact = get_tree_head_literal(NODE_BODY(node), 1, reg);
    }
  }

  return r;
}