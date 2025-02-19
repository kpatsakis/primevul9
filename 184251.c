compile_bag_node(BagNode* node, regex_t* reg, ScanEnv* env)
{
  int r, len;

  switch (node->type) {
  case BAG_MEMORY:
    r = compile_bag_memory_node(node, reg, env);
    break;

  case BAG_OPTION:
    r = compile_option_node(node, reg, env);
    break;

  case BAG_STOP_BACKTRACK:
    if (NODE_IS_STRICT_REAL_REPEAT(node)) {
      QuantNode* qn = QUANT_(NODE_BAG_BODY(node));
      r = compile_tree_n_times(NODE_QUANT_BODY(qn), qn->lower, reg, env);
      if (r != 0) return r;

      len = compile_length_tree(NODE_QUANT_BODY(qn), reg);
      if (len < 0) return len;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = SIZE_INC + len + OPSIZE_POP + OPSIZE_JUMP;

      r = compile_tree(NODE_QUANT_BODY(qn), reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_POP);
      if (r != 0) return r;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = -((int )OPSIZE_PUSH + len + (int )OPSIZE_POP);
    }
    else {
      MemNumType mid;

      ID_ENTRY(env, mid);
      r = add_op(reg, OP_MARK);
      if (r != 0) return r;
      COP(reg)->mark.id = mid;
      COP(reg)->mark.save_pos = 0;

      r = compile_tree(NODE_BAG_BODY(node), reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_CUT_TO_MARK);
      if (r != 0) return r;
      COP(reg)->cut_to_mark.id = mid;
      COP(reg)->cut_to_mark.restore_pos = 0;
    }
    break;

  case BAG_IF_ELSE:
    {
      int cond_len, then_len, else_len, jump_len;
      MemNumType mid;
      Node* cond = NODE_BAG_BODY(node);
      Node* Then = node->te.Then;
      Node* Else = node->te.Else;

      ID_ENTRY(env, mid);

      r = add_op(reg, OP_MARK);
      if (r != 0) return r;
      COP(reg)->mark.id = mid;
      COP(reg)->mark.save_pos = 0;

      cond_len = compile_length_tree(cond, reg);
      if (cond_len < 0) return cond_len;
      if (IS_NOT_NULL(Then)) {
        then_len = compile_length_tree(Then, reg);
        if (then_len < 0) return then_len;
      }
      else
        then_len = 0;

      jump_len = cond_len + then_len + OPSIZE_CUT_TO_MARK + OPSIZE_JUMP;

      r = add_op(reg, OP_PUSH);
      if (r != 0) return r;
      COP(reg)->push.addr = SIZE_INC + jump_len;

      r = compile_tree(cond, reg, env);
      if (r != 0) return r;
      r = add_op(reg, OP_CUT_TO_MARK);
      if (r != 0) return r;
      COP(reg)->cut_to_mark.id = mid;
      COP(reg)->cut_to_mark.restore_pos = 0;

      if (IS_NOT_NULL(Then)) {
        r = compile_tree(Then, reg, env);
        if (r != 0) return r;
      }

      if (IS_NOT_NULL(Else)) {
        else_len = compile_length_tree(Else, reg);
        if (else_len < 0) return else_len;
      }
      else
        else_len = 0;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = OPSIZE_CUT_TO_MARK + else_len + SIZE_INC;

      r = add_op(reg, OP_CUT_TO_MARK);
      if (r != 0) return r;
      COP(reg)->cut_to_mark.id = mid;
      COP(reg)->cut_to_mark.restore_pos = 0;

      if (IS_NOT_NULL(Else)) {
        r = compile_tree(Else, reg, env);
      }
    }
    break;
  }

  return r;
}