compile_anchor_look_behind_node(AnchorNode* node, regex_t* reg, ScanEnv* env)
{
  int r;

  if (node->char_min_len == node->char_max_len) {
    MemNumType mid;

    ID_ENTRY(env, mid);
    r = add_op(reg, OP_MARK);
    if (r != 0) return r;
    COP(reg)->mark.id = mid;
    COP(reg)->mark.save_pos = FALSE;

    r = add_op(reg, OP_STEP_BACK_START);
    if (r != 0) return r;
    COP(reg)->step_back_start.initial   = node->char_min_len;
    COP(reg)->step_back_start.remaining = 0;
    COP(reg)->step_back_start.addr      = 1;

    r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;

    r = add_op(reg, OP_CUT_TO_MARK);
    if (r != 0) return r;
    COP(reg)->cut_to_mark.id = mid;
    COP(reg)->cut_to_mark.restore_pos = FALSE;
  }
  else {
    MemNumType mid1, mid2;
    OnigLen diff;

    if (IS_NOT_NULL(node->lead_node)) {
      MinMaxCharLen ci;

      r = node_char_len(node->lead_node, reg, &ci, env);
      if (r < 0) return r;
      r = add_op(reg, OP_MOVE);
      if (r != 0) return r;
      COP(reg)->move.n = -((RelPositionType )ci.min);
      r = compile_tree(node->lead_node, reg, env);
      if (r != 0) return r;
    }

    ID_ENTRY(env, mid1);
    r = add_op(reg, OP_SAVE_VAL);
    if (r != 0) return r;
    COP(reg)->save_val.type = SAVE_RIGHT_RANGE;
    COP(reg)->save_val.id   = mid1;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_TO_S;

    ID_ENTRY(env, mid2);
    r = add_op(reg, OP_MARK);
    if (r != 0) return r;
    COP(reg)->mark.id = mid2;
    COP(reg)->mark.save_pos = FALSE;

    r = add_op(reg, OP_PUSH);
    if (r != 0) return r;
    COP(reg)->push.addr = SIZE_INC + OPSIZE_JUMP;

    r = add_op(reg, OP_JUMP);
    if (r != 0) return r;
    COP(reg)->jump.addr = SIZE_INC + OPSIZE_UPDATE_VAR + OPSIZE_FAIL;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_FROM_STACK;
    COP(reg)->update_var.id    = mid1;
    COP(reg)->update_var.clear = FALSE;
    r = add_op(reg, OP_FAIL);
    if (r != 0) return r;

    r = add_op(reg, OP_STEP_BACK_START);
    if (r != 0) return r;

    if (node->char_max_len != INFINITE_LEN)
      diff = node->char_max_len - node->char_min_len;
    else
      diff = INFINITE_LEN;

    COP(reg)->step_back_start.initial   = node->char_min_len;
    COP(reg)->step_back_start.remaining = diff;
    COP(reg)->step_back_start.addr      = 2;

    r = add_op(reg, OP_STEP_BACK_NEXT);
    if (r != 0) return r;

    r = compile_tree(NODE_ANCHOR_BODY(node), reg, env);
    if (r != 0) return r;

    r = add_op(reg, OP_CHECK_POSITION);
    if (r != 0) return r;
    COP(reg)->check_position.type = CHECK_POSITION_CURRENT_RIGHT_RANGE;

    r = add_op(reg, OP_CUT_TO_MARK);
    if (r != 0) return r;
    COP(reg)->cut_to_mark.id = mid2;
    COP(reg)->cut_to_mark.restore_pos = FALSE;

    r = add_op(reg, OP_UPDATE_VAR);
    if (r != 0) return r;
    COP(reg)->update_var.type = UPDATE_VAR_RIGHT_RANGE_FROM_STACK;
    COP(reg)->update_var.id    = mid1;
    COP(reg)->update_var.clear = TRUE;
  }

  return r;
}