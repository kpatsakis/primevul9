compile_quantifier_node(QuantNode* qn, regex_t* reg, ScanEnv* env)
{
  int i, r, mod_tlen;
  int infinite = IS_REPEAT_INFINITE(qn->upper);
  enum QuantBodyEmpty empty_info = qn->body_empty_info;
  int tlen = compile_length_tree(NODE_QUANT_BODY(qn), reg);

  if (tlen < 0) return tlen;
  if (tlen == 0) return 0;

  if (is_anychar_infinite_greedy(qn) &&
      (qn->lower <= 1 ||
       int_multiply_cmp(tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
    if (r != 0) return r;
    if (IS_NOT_NULL(qn->next_head_exact)) {
      if (IS_MULTILINE(CTYPE_OPTION(NODE_QUANT_BODY(qn), reg)))
        r = add_opcode(reg, OP_ANYCHAR_ML_STAR_PEEK_NEXT);
      else
        r = add_opcode(reg, OP_ANYCHAR_STAR_PEEK_NEXT);
      if (r != 0) return r;
      return add_bytes(reg, STR_(qn->next_head_exact)->s, 1);
    }
    else {
      if (IS_MULTILINE(CTYPE_OPTION(NODE_QUANT_BODY(qn), reg)))
        return add_opcode(reg, OP_ANYCHAR_ML_STAR);
      else
        return add_opcode(reg, OP_ANYCHAR_STAR);
    }
  }

  if (empty_info == QUANT_BODY_IS_NOT_EMPTY)
    mod_tlen = tlen;
  else
    mod_tlen = tlen + (SIZE_OP_EMPTY_CHECK_START + SIZE_OP_EMPTY_CHECK_END);

  if (infinite &&
      (qn->lower <= 1 ||
       int_multiply_cmp(tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    if (qn->lower == 1 && tlen > QUANTIFIER_EXPAND_LIMIT_SIZE) {
      if (qn->greedy) {
        if (IS_NOT_NULL(qn->head_exact))
          r = add_opcode_rel_addr(reg, OP_JUMP, SIZE_OP_PUSH_OR_JUMP_EXACT1);
        else if (IS_NOT_NULL(qn->next_head_exact))
          r = add_opcode_rel_addr(reg, OP_JUMP, SIZE_OP_PUSH_IF_PEEK_NEXT);
        else
          r = add_opcode_rel_addr(reg, OP_JUMP, SIZE_OP_PUSH);
      }
      else {
        r = add_opcode_rel_addr(reg, OP_JUMP, SIZE_OP_JUMP);
      }
      if (r != 0) return r;
    }
    else {
      r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
      if (r != 0) return r;
    }

    if (qn->greedy) {
      if (IS_NOT_NULL(qn->head_exact)) {
        r = add_opcode_rel_addr(reg, OP_PUSH_OR_JUMP_EXACT1,
                                mod_tlen + SIZE_OP_JUMP);
        if (r != 0) return r;
        add_bytes(reg, STR_(qn->head_exact)->s, 1);
        r = compile_tree_empty_check(NODE_QUANT_BODY(qn), reg, empty_info, env);
        if (r != 0) return r;
        r = add_opcode_rel_addr(reg, OP_JUMP,
           -(mod_tlen + (int )SIZE_OP_JUMP + (int )SIZE_OP_PUSH_OR_JUMP_EXACT1));
      }
      else if (IS_NOT_NULL(qn->next_head_exact)) {
        r = add_opcode_rel_addr(reg, OP_PUSH_IF_PEEK_NEXT,
                                mod_tlen + SIZE_OP_JUMP);
        if (r != 0) return r;
        add_bytes(reg, STR_(qn->next_head_exact)->s, 1);
        r = compile_tree_empty_check(NODE_QUANT_BODY(qn), reg, empty_info, env);
        if (r != 0) return r;
        r = add_opcode_rel_addr(reg, OP_JUMP,
           -(mod_tlen + (int )SIZE_OP_JUMP + (int )SIZE_OP_PUSH_IF_PEEK_NEXT));
      }
      else {
        r = add_opcode_rel_addr(reg, OP_PUSH, mod_tlen + SIZE_OP_JUMP);
        if (r != 0) return r;
        r = compile_tree_empty_check(NODE_QUANT_BODY(qn), reg, empty_info, env);
        if (r != 0) return r;
        r = add_opcode_rel_addr(reg, OP_JUMP,
                   -(mod_tlen + (int )SIZE_OP_JUMP + (int )SIZE_OP_PUSH));
      }
    }
    else {
      r = add_opcode_rel_addr(reg, OP_JUMP, mod_tlen);
      if (r != 0) return r;
      r = compile_tree_empty_check(NODE_QUANT_BODY(qn), reg, empty_info, env);
      if (r != 0) return r;
      r = add_opcode_rel_addr(reg, OP_PUSH, -(mod_tlen + (int )SIZE_OP_PUSH));
    }
  }
  else if (qn->upper == 0 && qn->is_refered != 0) { /* /(?<n>..){0}/ */
    r = add_opcode_rel_addr(reg, OP_JUMP, tlen);
    if (r != 0) return r;
    r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
  }
  else if (! infinite && qn->greedy &&
           (qn->upper == 1 ||
            int_multiply_cmp(tlen + SIZE_OP_PUSH, qn->upper,
                             QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    int n = qn->upper - qn->lower;

    r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
    if (r != 0) return r;

    for (i = 0; i < n; i++) {
      r = add_opcode_rel_addr(reg, OP_PUSH,
                              (n - i) * tlen + (n - i - 1) * SIZE_OP_PUSH);
      if (r != 0) return r;
      r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
      if (r != 0) return r;
    }
  }
  else if (! qn->greedy && qn->upper == 1 && qn->lower == 0) { /* '??' */
    r = add_opcode_rel_addr(reg, OP_PUSH, SIZE_OP_JUMP);
    if (r != 0) return r;
    r = add_opcode_rel_addr(reg, OP_JUMP, tlen);
    if (r != 0) return r;
    r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
  }
  else {
    r = compile_range_repeat_node(qn, mod_tlen, empty_info, reg, env);
  }
  return r;
}