ops_expand(regex_t* reg, int n)
{
#define MIN_OPS_EXPAND_SIZE   4

#ifdef USE_DIRECT_THREADED_CODE
  enum OpCode* cp;
#endif
  Operation* p;
  size_t size;

  if (n <= 0) n = MIN_OPS_EXPAND_SIZE;

  n += reg->ops_alloc;

  size = sizeof(Operation) * n;
  p = (Operation* )xrealloc(reg->ops, size);
  CHECK_NULL_RETURN_MEMERR(p);
  reg->ops = p;

#ifdef USE_DIRECT_THREADED_CODE
  size = sizeof(enum OpCode) * n;
  cp = (enum OpCode* )xrealloc(reg->ocs, size);
  CHECK_NULL_RETURN_MEMERR(cp);
  reg->ocs = cp;
#endif

  reg->ops_alloc = n;
  if (reg->ops_used == 0)
    reg->ops_curr = 0;
  else
    reg->ops_curr = reg->ops + (reg->ops_used - 1);

  return ONIG_NORMAL;
}