entry_repeat_range(regex_t* reg, int id, int lower, int upper, int ops_index)
{
#define REPEAT_RANGE_ALLOC  4

  RepeatRange* p;

  if (reg->repeat_range_alloc == 0) {
    p = (RepeatRange* )xmalloc(sizeof(RepeatRange) * REPEAT_RANGE_ALLOC);
    CHECK_NULL_RETURN_MEMERR(p);
    reg->repeat_range = p;
    reg->repeat_range_alloc = REPEAT_RANGE_ALLOC;
  }
  else if (reg->repeat_range_alloc <= id) {
    int n;
    n = reg->repeat_range_alloc + REPEAT_RANGE_ALLOC;
    p = (RepeatRange* )xrealloc(reg->repeat_range, sizeof(RepeatRange) * n);
    CHECK_NULL_RETURN_MEMERR(p);
    reg->repeat_range = p;
    reg->repeat_range_alloc = n;
  }
  else {
    p = reg->repeat_range;
  }

  p[id].lower    = lower;
  p[id].upper    = (IS_INFINITE_REPEAT(upper) ? 0x7fffffff : upper);
  p[id].u.offset = ops_index;
  return 0;
}