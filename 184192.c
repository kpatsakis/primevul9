compile_range_repeat_node(QuantNode* qn, int target_len, int emptiness,
                          regex_t* reg, ScanEnv* env)
{
  int r;
  int num_repeat = reg->num_repeat++;

  r = add_op(reg, qn->greedy ? OP_REPEAT : OP_REPEAT_NG);
  if (r != 0) return r;

  COP(reg)->repeat.id   = num_repeat;
  COP(reg)->repeat.addr = SIZE_INC + target_len + OPSIZE_REPEAT_INC;

  r = entry_repeat_range(reg, num_repeat, qn->lower, qn->upper,
                         COP_CURR_OFFSET(reg) + OPSIZE_REPEAT);
  if (r != 0) return r;

  r = compile_quant_body_with_empty_check(qn, reg, env);
  if (r != 0) return r;

  r = add_op(reg, qn->greedy ? OP_REPEAT_INC : OP_REPEAT_INC_NG);
  if (r != 0) return r;

  COP(reg)->repeat_inc.id = num_repeat;
  return r;
}