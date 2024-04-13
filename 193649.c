compile_enclosure_memory_node(EnclosureNode* node, regex_t* reg, ScanEnv* env)
{
  int r;
  int len;

#ifdef USE_CALL
  if (node->m.regnum == 0 && NODE_IS_CALLED(node)) {
    r = add_opcode(reg, OP_CALL);
    if (r != 0) return r;
    node->m.called_addr = BB_GET_OFFSET_POS(reg) + SIZE_ABSADDR + SIZE_OP_JUMP;
    NODE_STATUS_ADD(node, NST_ADDR_FIXED);
    r = add_abs_addr(reg, (int )node->m.called_addr);
    if (r != 0) return r;
    len = compile_length_tree(NODE_ENCLOSURE_BODY(node), reg);
    len += SIZE_OP_RETURN;
    r = add_opcode_rel_addr(reg, OP_JUMP, len);
    if (r != 0) return r;

    r = compile_tree(NODE_ENCLOSURE_BODY(node), reg, env);
    if (r != 0) return r;
    r = add_opcode(reg, OP_RETURN);
    return r;
  }

  if (NODE_IS_CALLED(node)) {
    r = add_opcode(reg, OP_CALL);
    if (r != 0) return r;
    node->m.called_addr = BB_GET_OFFSET_POS(reg) + SIZE_ABSADDR + SIZE_OP_JUMP;
    NODE_STATUS_ADD(node, NST_ADDR_FIXED);
    r = add_abs_addr(reg, (int )node->m.called_addr);
    if (r != 0) return r;
    len = compile_length_tree(NODE_ENCLOSURE_BODY(node), reg);
    len += (SIZE_OP_MEMORY_START_PUSH + SIZE_OP_RETURN);
    if (MEM_STATUS_AT0(reg->bt_mem_end, node->m.regnum))
      len += (NODE_IS_RECURSION(node)
              ? SIZE_OP_MEMORY_END_PUSH_REC : SIZE_OP_MEMORY_END_PUSH);
    else
      len += (NODE_IS_RECURSION(node)
              ? SIZE_OP_MEMORY_END_REC : SIZE_OP_MEMORY_END);

    r = add_opcode_rel_addr(reg, OP_JUMP, len);
    if (r != 0) return r;
  }
#endif

  if (MEM_STATUS_AT0(reg->bt_mem_start, node->m.regnum))
    r = add_opcode(reg, OP_MEMORY_START_PUSH);
  else
    r = add_opcode(reg, OP_MEMORY_START);
  if (r != 0) return r;
  r = add_mem_num(reg, node->m.regnum);
  if (r != 0) return r;
  r = compile_tree(NODE_ENCLOSURE_BODY(node), reg, env);
  if (r != 0) return r;

#ifdef USE_CALL
  if (MEM_STATUS_AT0(reg->bt_mem_end, node->m.regnum))
    r = add_opcode(reg, (NODE_IS_RECURSION(node)
                         ? OP_MEMORY_END_PUSH_REC : OP_MEMORY_END_PUSH));
  else
    r = add_opcode(reg, (NODE_IS_RECURSION(node)
                         ? OP_MEMORY_END_REC : OP_MEMORY_END));
  if (r != 0) return r;
  r = add_mem_num(reg, node->m.regnum);
  if (NODE_IS_CALLED(node)) {
    if (r != 0) return r;
    r = add_opcode(reg, OP_RETURN);
  }
#else
  if (MEM_STATUS_AT0(reg->bt_mem_end, node->m.regnum))
    r = add_opcode(reg, OP_MEMORY_END_PUSH);
  else
    r = add_opcode(reg, OP_MEMORY_END);
  if (r != 0) return r;
  r = add_mem_num(reg, node->m.regnum);
#endif

  return r;
}