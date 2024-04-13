ops_init(regex_t* reg, int init_alloc_size)
{
  Operation* p;
  size_t size;

  if (init_alloc_size > 0) {
    size = sizeof(Operation) * init_alloc_size;
    p = (Operation* )xrealloc(reg->ops, size);
    CHECK_NULL_RETURN_MEMERR(p);
    reg->ops = p;
#ifdef USE_DIRECT_THREADED_CODE
    {
      enum OpCode* cp;
      size = sizeof(enum OpCode) * init_alloc_size;
      cp = (enum OpCode* )xrealloc(reg->ocs, size);
      CHECK_NULL_RETURN_MEMERR(cp);
      reg->ocs = cp;
    }
#endif
  }
  else {
    reg->ops = (Operation* )0;
#ifdef USE_DIRECT_THREADED_CODE
    reg->ocs = (enum OpCode* )0;
#endif
  }

  reg->ops_curr  = 0; /* !!! not yet done ops_new() */
  reg->ops_alloc = init_alloc_size;
  reg->ops_used  = 0;

  return ONIG_NORMAL;
}