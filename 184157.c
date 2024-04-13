compile_quant_body_with_empty_check(QuantNode* qn, regex_t* reg, ScanEnv* env)
{
  int r;
  int saved_num_empty_check;
  int emptiness;
  Node* body;

  body = NODE_BODY((Node* )qn);
  emptiness = qn->emptiness;
  saved_num_empty_check = reg->num_empty_check;

  if (emptiness != BODY_IS_NOT_EMPTY) {
    r = add_op(reg, OP_EMPTY_CHECK_START);
    if (r != 0) return r;
    COP(reg)->empty_check_start.mem = reg->num_empty_check; /* NULL CHECK ID */
    reg->num_empty_check++;
  }

  r = compile_tree(body, reg, env);
  if (r != 0) return r;

  if (emptiness != BODY_IS_NOT_EMPTY) {
    if (emptiness == BODY_MAY_BE_EMPTY)
      r = add_op(reg, OP_EMPTY_CHECK_END);
    else if (emptiness == BODY_MAY_BE_EMPTY_MEM) {
      if (NODE_IS_EMPTY_STATUS_CHECK(qn) != 0)
        r = add_op(reg, OP_EMPTY_CHECK_END_MEMST);
      else
        r = add_op(reg, OP_EMPTY_CHECK_END);
    }
#ifdef USE_CALL
    else if (emptiness == BODY_MAY_BE_EMPTY_REC)
      r = add_op(reg, OP_EMPTY_CHECK_END_MEMST_PUSH);
#endif

    if (r != 0) return r;
    COP(reg)->empty_check_end.mem = saved_num_empty_check; /* NULL CHECK ID */
  }
  return r;
}