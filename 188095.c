add_code_range_to_buf0(BBuf** pbuf, ScanEnv* env, OnigCodePoint from, OnigCodePoint to,
	int checkdup)
{
  int r, inc_n, pos;
  OnigCodePoint low, high, bound, x;
  OnigCodePoint n, *data;
  BBuf* bbuf;

  if (from > to) {
    n = from; from = to; to = n;
  }

  if (IS_NULL(*pbuf)) {
    r = new_code_range(pbuf);
    if (r) return r;
    bbuf = *pbuf;
    n = 0;
  }
  else {
    bbuf = *pbuf;
    GET_CODE_POINT(n, bbuf->p);
  }
  data = (OnigCodePoint* )(bbuf->p);
  data++;

  bound = (from == 0) ? 0 : n;
  for (low = 0; low < bound; ) {
    x = (low + bound) >> 1;
    if (from - 1 > data[x*2 + 1])
      low = x + 1;
    else
      bound = x;
  }

  high = (to == ONIG_LAST_CODE_POINT) ? n : low;
  for (bound = n; high < bound; ) {
    x = (high + bound) >> 1;
    if (to + 1 >= data[x*2])
      high = x + 1;
    else
      bound = x;
  }
  /* data[(low-1)*2+1] << from <= data[low*2]
   * data[(high-1)*2+1] <= to << data[high*2]
   */

  inc_n = low + 1 - high;
  if (n + inc_n > ONIG_MAX_MULTI_BYTE_RANGES_NUM)
    return ONIGERR_TOO_MANY_MULTI_BYTE_RANGES;

  if (inc_n != 1) {
    if (checkdup && from <= data[low*2+1]
	&& (data[low*2] <= from || data[low*2+1] <= to))
      CC_DUP_WARN(env, from, to);
    if (from > data[low*2])
      from = data[low*2];
    if (to < data[(high - 1)*2 + 1])
      to = data[(high - 1)*2 + 1];
  }

  if (inc_n != 0) {
    int from_pos = SIZE_CODE_POINT * (1 + high * 2);
    int to_pos   = SIZE_CODE_POINT * (1 + (low + 1) * 2);

    if (inc_n > 0) {
      if (high < n) {
	int size = (n - high) * 2 * SIZE_CODE_POINT;
	BBUF_MOVE_RIGHT(bbuf, from_pos, to_pos, size);
      }
    }
    else {
      BBUF_MOVE_LEFT_REDUCE(bbuf, from_pos, to_pos);
    }
  }

  pos = SIZE_CODE_POINT * (1 + low * 2);
  BBUF_ENSURE_SIZE(bbuf, pos + SIZE_CODE_POINT * 2);
  BBUF_WRITE_CODE_POINT(bbuf, pos, from);
  BBUF_WRITE_CODE_POINT(bbuf, pos + SIZE_CODE_POINT, to);
  n += inc_n;
  BBUF_WRITE_CODE_POINT(bbuf, 0, n);

  return 0;
}