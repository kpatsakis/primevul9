compile_quantifier_node(QuantNode* qn, regex_t* reg, ScanEnv* env)
{
  int i, r, mod_tlen;
  int infinite = IS_INFINITE_REPEAT(qn->upper);
  enum BodyEmptyType emptiness = qn->emptiness;
  int tlen = compile_length_tree(NODE_QUANT_BODY(qn), reg);

  if (tlen < 0) return tlen;
  if (tlen == 0) return 0;

  if (is_anychar_infinite_greedy(qn) &&
      (qn->lower <= 1 ||
       len_multiply_cmp((OnigLen )tlen, qn->lower,
                        QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
    if (r != 0) return r;
    if (IS_NOT_NULL(qn->next_head_exact)) {
      r = add_op(reg, NODE_IS_MULTILINE(NODE_QUANT_BODY(qn)) ?
                 OP_ANYCHAR_ML_STAR_PEEK_NEXT : OP_ANYCHAR_STAR_PEEK_NEXT);
      if (r != 0) return r;

      COP(reg)->anychar_star_peek_next.c = STR_(qn->next_head_exact)->s[0];
      return 0;
    }
    else {
      r = add_op(reg, NODE_IS_MULTILINE(NODE_QUANT_BODY(qn)) ?
                 OP_ANYCHAR_ML_STAR : OP_ANYCHAR_STAR);
      return r;
    }
  }

  mod_tlen = tlen;
  if (emptiness != BODY_IS_NOT_EMPTY)
    mod_tlen += OPSIZE_EMPTY_CHECK_START + OPSIZE_EMPTY_CHECK_END;

  if (infinite &&
      (qn->lower <= 1 ||
       len_multiply_cmp((OnigLen )tlen, qn->lower,
                        QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    int addr;

    if (qn->lower == 1 && tlen > QUANTIFIER_EXPAND_LIMIT_SIZE) {
      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      if (qn->greedy) {
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
        if (IS_NOT_NULL(qn->head_exact))
          COP(reg)->jump.addr = OPSIZE_PUSH_OR_JUMP_EXACT1 + SIZE_INC;
        else
#endif
        if (IS_NOT_NULL(qn->next_head_exact))
          COP(reg)->jump.addr = OPSIZE_PUSH_IF_PEEK_NEXT + SIZE_INC;
        else
          COP(reg)->jump.addr = OPSIZE_PUSH + SIZE_INC;
      }
      else {
        COP(reg)->jump.addr = OPSIZE_JUMP + SIZE_INC;
      }
    }
    else {
      r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
      if (r != 0) return r;
    }

    if (qn->greedy) {
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
      if (IS_NOT_NULL(qn->head_exact)) {
        r = add_op(reg, OP_PUSH_OR_JUMP_EXACT1);
        if (r != 0) return r;
        COP(reg)->push_or_jump_exact1.addr = SIZE_INC + mod_tlen + OPSIZE_JUMP;
        COP(reg)->push_or_jump_exact1.c    = STR_(qn->head_exact)->s[0];

        r = compile_quant_body_with_empty_check(qn, reg, env);
        if (r != 0) return r;

        addr = -(mod_tlen + (int )OPSIZE_PUSH_OR_JUMP_EXACT1);
      }
      else
#endif
      if (IS_NOT_NULL(qn->next_head_exact)) {
        r = add_op(reg, OP_PUSH_IF_PEEK_NEXT);
        if (r != 0) return r;
        COP(reg)->push_if_peek_next.addr = SIZE_INC + mod_tlen + OPSIZE_JUMP;
        COP(reg)->push_if_peek_next.c    = STR_(qn->next_head_exact)->s[0];

        r = compile_quant_body_with_empty_check(qn, reg, env);
        if (r != 0) return r;

        addr = -(mod_tlen + (int )OPSIZE_PUSH_IF_PEEK_NEXT);
      }
      else {
        r = add_op(reg, OP_PUSH);
        if (r != 0) return r;
        COP(reg)->push.addr = SIZE_INC + mod_tlen + OPSIZE_JUMP;

        r = compile_quant_body_with_empty_check(qn, reg, env);
        if (r != 0) return r;

        addr = -(mod_tlen + (int )OPSIZE_PUSH);
      }

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = addr;
    }
    else {
      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = mod_tlen + SIZE_INC;

      r = compile_quant_body_with_empty_check(qn, reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = -mod_tlen;
    }
  }
  else if (qn->upper == 0) {
    if (qn->include_referred != 0) { /* /(?<n>..){0}/ */
      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = tlen + SIZE_INC;

      r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
    }
    else {
      /* Nothing output */
      r = 0;
    }
  }
  else if (! infinite && qn->greedy &&
           (qn->upper == 1 ||
            len_multiply_cmp((OnigLen )tlen + OPSIZE_PUSH, qn->upper,
                             QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    int n = qn->upper - qn->lower;

    r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
    if (r != 0) return r;

    for (i = 0; i < n; i++) {
      int v = onig_positive_int_multiply(n - i, tlen + OPSIZE_PUSH);
      if (v < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = v;

      r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
      if (r != 0) return r;
    }
  }
  else if (! qn->greedy && qn->upper == 1 && qn->lower == 0) { /* '??' */
    r = add_op(reg, OP_PUSH);
    if (r != 0) return r;
    COP(reg)->push.addr = SIZE_INC + OPSIZE_JUMP;

    r = add_op(reg, OP_JUMP);
    if (r != 0) return r;
    COP(reg)->jump.addr = tlen + SIZE_INC;

    r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
  }
  else {
    r = compile_range_repeat_node(qn, mod_tlen, emptiness, reg, env);
  }
  return r;
}