static uint64_t htonll (uint64_t val) /* {{{ */
{
  static int config = 0;

  uint32_t hi;
  uint32_t lo;

  if (config == 0)
  {
    uint16_t h = 0x1234;
    uint16_t n = htons (h);

    if (h == n)
      config = 1;
    else
      config = 2;
  }

  if (config == 1)
    return (val);

  hi = (uint32_t) (val >> 32);
  lo = (uint32_t) (val & 0x00000000FFFFFFFF);

  hi = htonl (hi);
  lo = htonl (lo);

  return ((((uint64_t) lo) << 32) | ((uint64_t) hi));
} /* }}} uint64_t htonll */