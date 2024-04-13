compile_bag_memory_node(BagNode* node, regex_t* reg, ScanEnv* env)
{
  int r;

#ifdef USE_CALL
  if (NODE_IS_CALLED(node)) {
    int len;

    r = add_op(reg, OP_CALL);
    if (r != 0) return r;

    node->m.called_addr = COP_CURR_OFFSET(reg) + 1 + OPSIZE_JUMP;
    NODE_STATUS_ADD(node, FIXED_ADDR);
    COP(reg)->call.addr = (int )node->m.called_addr;

    if (node->m.regnum == 0) {
      len = compile_length_tree(NODE_BAG_BODY(node), reg);
      len += OPSIZE_RETURN;

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = len + SIZE_INC;

      r = compile_tree(NODE_BAG_BODY(node), reg, env);
      if (r != 0) return r;

      r = add_op(reg, OP_RETURN);
      return r;
    }
    else {
      len = compile_length_tree(NODE_BAG_BODY(node), reg);
      len += (OPSIZE_MEM_START_PUSH + OPSIZE_RETURN);
      if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
        len += (NODE_IS_RECURSION(node)
                ? OPSIZE_MEM_END_PUSH_REC : OPSIZE_MEM_END_PUSH);
      else
        len += (NODE_IS_RECURSION(node) ? OPSIZE_MEM_END_REC : OPSIZE_MEM_END);

      r = add_op(reg, OP_JUMP);
      if (r != 0) return r;
      COP(reg)->jump.addr = len + SIZE_INC;
    }
  }
#endif

  if (MEM_STATUS_AT0(reg->push_mem_start, node->m.regnum))
    r = add_op(reg, OP_MEM_START_PUSH);
  else
    r = add_op(reg, OP_MEM_START);
  if (r != 0) return r;
  COP(reg)->memory_start.num = node->m.regnum;

  r = compile_tree(NODE_BAG_BODY(node), reg, env);
  if (r != 0) return r;

#ifdef USE_CALL
  if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
    r = add_op(reg, (NODE_IS_RECURSION(node)
                     ? OP_MEM_END_PUSH_REC : OP_MEM_END_PUSH));
  else
    r = add_op(reg, (NODE_IS_RECURSION(node) ? OP_MEM_END_REC : OP_MEM_END));
  if (r != 0) return r;
  COP(reg)->memory_end.num = node->m.regnum;

  if (NODE_IS_CALLED(node)) {
    r = add_op(reg, OP_RETURN);
  }
#else
  if (MEM_STATUS_AT0(reg->push_mem_end, node->m.regnum))
    r = add_op(reg, OP_MEM_END_PUSH);
  else
    r = add_op(reg, OP_MEM_END);
  if (r != 0) return r;
  COP(reg)->memory_end.num = node->m.regnum;
#endif

  return r;
}