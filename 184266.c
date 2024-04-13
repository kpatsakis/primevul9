compile_anchor_node(AnchorNode* node, regex_t* reg, ScanEnv* env)
{
  int r, len;
  enum OpCode op;
  MemNumType mid;

  switch (node->type) {
  case ANCR_BEGIN_BUF:      r = add_op(reg, OP_BEGIN_BUF);      break;
  case ANCR_END_BUF:        r = add_op(reg, OP_END_BUF);        break;
  case ANCR_BEGIN_LINE:     r = add_op(reg, OP_BEGIN_LINE);     break;
  case ANCR_END_LINE:       r = add_op(reg, OP_END_LINE);       break;
  case ANCR_SEMI_END_BUF:   r = add_op(reg, OP_SEMI_END_BUF);   break;
  case ANCR_BEGIN_POSITION:
    r = add_op(reg, OP_CHECK_POSITION);
    if (r != 0) return r;
    COP(reg)->check_position.type = CHECK_POSITION_SEARCH_START;
    break;

  case ANCR_WORD_BOUNDARY:
    op = OP_WORD_BOUNDARY;
  word:
    r = add_op(reg, op);
    if (r != 0) return r;
    COP(reg)->word_boundary.mode = (ModeType )node->ascii_mode;
    break;

  case ANCR_NO_WORD_BOUNDARY:
    op = OP_NO_WORD_BOUNDARY; goto word;
    break;
#ifdef USE_WORD_BEGIN_END
  case ANCR_WORD_BEGIN:
    op = OP_WORD_BEGIN; goto word;
    break;
  case ANCR_WORD_END:
    op = OP_WORD_END; goto word;
    break;
#endif

  case ANCR_TEXT_SEGMENT_BOUNDARY:
  case ANCR_NO_TEXT_SEGMENT_BOUNDARY:
    {
      enum TextSegmentBoundaryType type;

      r = add_op(reg, OP_TEXT_SEGMENT_BOUNDARY);
      if (r != 0) return r;

      type = EXTENDED_GRAPHEME_CLUSTER_BOUNDARY;
#ifdef USE_UNICODE_WORD_BREAK
      if (NODE_IS_TEXT_SEGMENT_WORD(node))
        type = WORD_BOUNDARY;
#endif

      COP(reg)->text_segment_boundary.type = type;
      COP(reg)->text_segment_boundary.not =
        (node->type == ANCR_NO_TEXT_SEGMENT_BOUNDARY ? 1 : 0);
    }
    break;

  case ANCR_PREC_READ:
    {
      ID_ENTRY(env, mid);
      r = add_op(reg, OP_MARK);
      if (r != 0) return r;
      COP(reg)->mark.id = mid;
      COP(reg)->mark.save_pos = TRUE;

      r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_CUT_TO_MARK);
      if (r != 0) return r;
      COP(reg)->cut_to_mark.id = mid;
      COP(reg)->cut_to_mark.restore_pos = TRUE;
    }
    break;

  case ANCR_PREC_READ_NOT:
    {
      len = compile_length_tree(NODE_ANCHOR_BODY(node), reg);
      if (len < 0) return len;

      ID_ENTRY(env, mid);
      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = SIZE_INC + OPSIZE_MARK + len +
                            OPSIZE_POP_TO_MARK + OPSIZE_POP + OPSIZE_FAIL;

      r = add_op(reg, OP_MARK);
      if (r != 0) return r;
      COP(reg)->mark.id = mid;
      COP(reg)->mark.save_pos = FALSE;

      r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_POP_TO_MARK);
      if (r != 0) return r;
      COP(reg)->pop_to_mark.id = mid;

      r = add_op(reg, OP_POP);
      if (r != 0) return r;
      r = add_op(reg, OP_FAIL);
    }
    break;

  case ANCR_LOOK_BEHIND:
    r = compile_anchor_look_behind_node(node, reg, env);
    break;

  case ANCR_LOOK_BEHIND_NOT:
    r = compile_anchor_look_behind_not_node(node, reg, env);
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return r;
}