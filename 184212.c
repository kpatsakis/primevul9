compile_length_bag_node(BagNode* node, regex_t* reg)
{
  int len;
  int tlen;

  if (node->type == BAG_OPTION)
    return compile_length_option_node(node, reg);

  if (NODE_BAG_BODY(node)) {
    tlen = compile_length_tree(NODE_BAG_BODY(node), reg);
    if (tlen < 0) return tlen;
  }
  else
    tlen = 0;

  switch (node->type) {
  case BAG_MEMORY:
#ifdef USE_CALL

    if (node->m.regnum == 0 && NODE_IS_CALLED(node)) {
      len = tlen + OPSIZE_CALL + OPSIZE_JUMP + OPSIZE_RETURN;
      return len;
    }

    if (NODE_IS_CALLED(node)) {
      len = OPSIZE_MEM_START_PUSH + tlen
        + OPSIZE_CALL + OPSIZE_JUMP + OPSIZE_RETURN;
      if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
        len += (NODE_IS_RECURSION(node)
                ? OPSIZE_MEM_END_PUSH_REC : OPSIZE_MEM_END_PUSH);
      else
        len += (NODE_IS_RECURSION(node)
                ? OPSIZE_MEM_END_REC : OPSIZE_MEM_END);
    }
    else if (NODE_IS_RECURSION(node)) {
      len = OPSIZE_MEM_START_PUSH;
      len += tlen + (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum)
                     ? OPSIZE_MEM_END_PUSH_REC : OPSIZE_MEM_END_REC);
    }
    else
#endif
    {
      if (MEM_STATUS_AT0(reg->push_mem_start, node->m.regnum))
        len = OPSIZE_MEM_START_PUSH;
      else
        len = OPSIZE_MEM_START;

      len += tlen + (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum)
                     ? OPSIZE_MEM_END_PUSH : OPSIZE_MEM_END);
    }
    break;

  case BAG_STOP_BACKTRACK:
    if (NODE_IS_STRICT_REAL_REPEAT(node)) {
      int v;
      QuantNode* qn;

      qn = QUANT_(NODE_BAG_BODY(node));
      tlen = compile_length_tree(NODE_QUANT_BODY(qn), reg);
      if (tlen < 0) return tlen;

      v = onig_positive_int_multiply(qn->lower, tlen);
      if (v < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;
      len = v + OPSIZE_PUSH + tlen + OPSIZE_POP + OPSIZE_JUMP;
    }
    else {
      len = OPSIZE_MARK + tlen + OPSIZE_CUT_TO_MARK;
    }
    break;

  case BAG_IF_ELSE:
    {
      Node* cond = NODE_BAG_BODY(node);
      Node* Then = node->te.Then;
      Node* Else = node->te.Else;

      len = compile_length_tree(cond, reg);
      if (len < 0) return len;
      len += OPSIZE_PUSH + OPSIZE_MARK + OPSIZE_CUT_TO_MARK;

      if (IS_NOT_NULL(Then)) {
        tlen = compile_length_tree(Then, reg);
        if (tlen < 0) return tlen;
        len += tlen;
      }

      len += OPSIZE_JUMP + OPSIZE_CUT_TO_MARK;

      if (IS_NOT_NULL(Else)) {
        tlen = compile_length_tree(Else, reg);
        if (tlen < 0) return tlen;
        len += tlen;
      }
    }
    break;

  case BAG_OPTION:
    /* never come here, but set for escape warning */
    len = 0;
    break;
  }

  return len;
}