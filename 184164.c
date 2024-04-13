add_op(regex_t* reg, int opcode)
{
  int r;

  r = ops_new(reg);
  if (r != ONIG_NORMAL) return r;

#ifdef USE_DIRECT_THREADED_CODE
  *(reg->ocs + (reg->ops_curr - reg->ops)) = opcode;
#else
  reg->ops_curr->opcode = opcode;
#endif

  return 0;
}