compile_length_quantifier_node(QuantNode* qn, regex_t* reg)
{
  int len, mod_tlen;
  int infinite = IS_INFINITE_REPEAT(qn->upper);
  enum BodyEmptyType emptiness = qn->emptiness;
  int tlen = compile_length_tree(NODE_QUANT_BODY(qn), reg);

  if (tlen < 0) return tlen;
  if (tlen == 0) return 0;

  /* anychar repeat */
  if (is_anychar_infinite_greedy(qn)) {
    if (qn->lower <= 1 ||
        len_multiply_cmp((OnigLen )tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0) {
      if (IS_NOT_NULL(qn->next_head_exact))
        return OPSIZE_ANYCHAR_STAR_PEEK_NEXT + tlen * qn->lower;
      else
        return OPSIZE_ANYCHAR_STAR + tlen * qn->lower;
    }
  }

  mod_tlen = tlen;
  if (emptiness != BODY_IS_NOT_EMPTY)
    mod_tlen += OPSIZE_EMPTY_CHECK_START + OPSIZE_EMPTY_CHECK_END;

  if (infinite &&
      (qn->lower <= 1 ||
       len_multiply_cmp(tlen, qn->lower, QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    if (qn->lower == 1 && tlen > QUANTIFIER_EXPAND_LIMIT_SIZE) {
      len = OPSIZE_JUMP;
    }
    else {
      len = tlen * qn->lower;
    }

    if (qn->greedy) {
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
      if (IS_NOT_NULL(qn->head_exact))
        len += OPSIZE_PUSH_OR_JUMP_EXACT1 + mod_tlen + OPSIZE_JUMP;
      else
#endif
      if (IS_NOT_NULL(qn->next_head_exact))
        len += OPSIZE_PUSH_IF_PEEK_NEXT + mod_tlen + OPSIZE_JUMP;
      else
        len += OPSIZE_PUSH + mod_tlen + OPSIZE_JUMP;
    }
    else
      len += OPSIZE_JUMP + mod_tlen + OPSIZE_PUSH;
  }
  else if (qn->upper == 0) {
    if (qn->include_referred != 0) { /* /(?<n>..){0}/ */
      len = OPSIZE_JUMP + tlen;
    }
    else
      len = 0;
  }
  else if (!infinite && qn->greedy &&
           (qn->upper == 1 ||
            len_multiply_cmp((OnigLen )tlen + OPSIZE_PUSH, qn->upper,
                             QUANTIFIER_EXPAND_LIMIT_SIZE) <= 0)) {
    len = tlen * qn->lower;
    len += (OPSIZE_PUSH + tlen) * (qn->upper - qn->lower);
  }
  else if (!qn->greedy && qn->upper == 1 && qn->lower == 0) { /* '??' */
    len = OPSIZE_PUSH + OPSIZE_JUMP + tlen;
  }
  else {
    len = OPSIZE_REPEAT_INC + mod_tlen + OPSIZE_REPEAT;
  }

  return len;
}