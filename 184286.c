set_optimize_map(regex_t* reg, OptMap* m)
{
  int i;

  for (i = 0; i < CHAR_MAP_SIZE; i++)
    reg->map[i] = m->map[i];

  reg->optimize   = OPTIMIZE_MAP;
  reg->dist_min   = m->mm.min;
  reg->dist_max   = m->mm.max;

  if (reg->dist_min != INFINITE_LEN) {
    reg->threshold_len = reg->dist_min + ONIGENC_MBC_MINLEN(reg->enc);
  }
}