ops_calc_size_of_string_pool(regex_t* reg)
{
  int i;
  int total;

  if (IS_NULL(reg->ops)) return 0;

  total = 0;
  for (i = 0; i < (int )reg->ops_used; i++) {
    enum OpCode opcode;
    Operation* op;

    op = reg->ops + i;
#ifdef USE_DIRECT_THREADED_CODE
    opcode = *(reg->ocs + i);
#else
    opcode = op->opcode;
#endif

    switch (opcode) {
    case OP_STR_MBN:
      total += op->exact_len_n.len * op->exact_len_n.n;
      break;
    case OP_STR_N:
    case OP_STR_MB2N:
      total += op->exact_n.n * 2;
      break;
    case OP_STR_MB3N:
      total += op->exact_n.n * 3;
      break;

    default:
      break;
    }
  }

  return total;
}