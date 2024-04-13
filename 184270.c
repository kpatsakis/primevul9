compile_length_anchor_node(AnchorNode* node, regex_t* reg)
{
  int len;
  int tlen = 0;

  if (IS_NOT_NULL(NODE_ANCHOR_BODY(node))) {
    tlen = compile_length_tree(NODE_ANCHOR_BODY(node), reg);
    if (tlen < 0) return tlen;
  }

  switch (node->type) {
  case ANCR_PREC_READ:
    len = OPSIZE_MARK + tlen + OPSIZE_CUT_TO_MARK;
    break;
  case ANCR_PREC_READ_NOT:
    len = OPSIZE_PUSH + OPSIZE_MARK + tlen + OPSIZE_POP_TO_MARK + OPSIZE_POP + OPSIZE_FAIL;
    break;
  case ANCR_LOOK_BEHIND:
    if (node->char_min_len == node->char_max_len)
      len = OPSIZE_MARK + OPSIZE_STEP_BACK_START + tlen + OPSIZE_CUT_TO_MARK;
    else {
      len = OPSIZE_SAVE_VAL + OPSIZE_UPDATE_VAR + OPSIZE_MARK + OPSIZE_PUSH + OPSIZE_UPDATE_VAR + OPSIZE_FAIL + OPSIZE_JUMP + OPSIZE_STEP_BACK_START + OPSIZE_STEP_BACK_NEXT + tlen + OPSIZE_CHECK_POSITION + OPSIZE_CUT_TO_MARK + OPSIZE_UPDATE_VAR;

      if (IS_NOT_NULL(node->lead_node)) {
        int llen = compile_length_tree(node->lead_node, reg);
        if (llen < 0) return llen;

        len += OPSIZE_MOVE + llen;
      }
    }
    break;
  case ANCR_LOOK_BEHIND_NOT:
    if (node->char_min_len == node->char_max_len)
      len = OPSIZE_MARK + OPSIZE_PUSH + OPSIZE_STEP_BACK_START + tlen + OPSIZE_POP_TO_MARK + OPSIZE_FAIL + OPSIZE_POP;
    else {
      len = OPSIZE_SAVE_VAL + OPSIZE_UPDATE_VAR + OPSIZE_MARK + OPSIZE_PUSH + OPSIZE_STEP_BACK_START + OPSIZE_STEP_BACK_NEXT + tlen + OPSIZE_CHECK_POSITION + OPSIZE_POP_TO_MARK + OPSIZE_UPDATE_VAR + OPSIZE_POP + OPSIZE_FAIL + OPSIZE_UPDATE_VAR + OPSIZE_POP + OPSIZE_POP;

      if (IS_NOT_NULL(node->lead_node)) {
        int llen = compile_length_tree(node->lead_node, reg);
        if (llen < 0) return llen;

        len += OPSIZE_MOVE + llen;
      }
    }
    break;

  case ANCR_WORD_BOUNDARY:
  case ANCR_NO_WORD_BOUNDARY:
#ifdef USE_WORD_BEGIN_END
  case ANCR_WORD_BEGIN:
  case ANCR_WORD_END:
#endif
    len = OPSIZE_WORD_BOUNDARY;
    break;

  case ANCR_TEXT_SEGMENT_BOUNDARY:
  case ANCR_NO_TEXT_SEGMENT_BOUNDARY:
    len = SIZE_OPCODE;
    break;

  default:
    len = SIZE_OPCODE;
    break;
  }

  return len;
}