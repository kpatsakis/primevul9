ops_free(regex_t* reg)
{
  int i;

  if (IS_NULL(reg->ops)) return ;

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
      if (! is_in_string_pool(reg, op->exact_len_n.s))
        xfree(op->exact_len_n.s);
      break;
    case OP_STR_N: case OP_STR_MB2N: case OP_STR_MB3N:
      if (! is_in_string_pool(reg, op->exact_n.s))
        xfree(op->exact_n.s);
      break;
    case OP_STR_1: case OP_STR_2: case OP_STR_3: case OP_STR_4:
    case OP_STR_5: case OP_STR_MB2N1: case OP_STR_MB2N2:
    case OP_STR_MB2N3:
      break;

    case OP_CCLASS_NOT: case OP_CCLASS:
      xfree(op->cclass.bsp);
      break;

    case OP_CCLASS_MB_NOT: case OP_CCLASS_MB:
      xfree(op->cclass_mb.mb);
      break;
    case OP_CCLASS_MIX_NOT: case OP_CCLASS_MIX:
      xfree(op->cclass_mix.mb);
      xfree(op->cclass_mix.bsp);
      break;

    case OP_BACKREF1: case OP_BACKREF2: case OP_BACKREF_N: case OP_BACKREF_N_IC:
      break;
    case OP_BACKREF_MULTI:      case OP_BACKREF_MULTI_IC:
    case OP_BACKREF_CHECK:
#ifdef USE_BACKREF_WITH_LEVEL
    case OP_BACKREF_WITH_LEVEL:
    case OP_BACKREF_WITH_LEVEL_IC:
    case OP_BACKREF_CHECK_WITH_LEVEL:
#endif
      if (op->backref_general.num != 1)
        xfree(op->backref_general.ns);
      break;

    default:
      break;
    }
  }

  xfree(reg->ops);
#ifdef USE_DIRECT_THREADED_CODE
  xfree(reg->ocs);
  reg->ocs = 0;
#endif

  reg->ops = 0;
  reg->ops_curr  = 0;
  reg->ops_alloc = 0;
  reg->ops_used  = 0;
}