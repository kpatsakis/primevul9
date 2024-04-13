print_optimize_info(FILE* f, regex_t* reg)
{
  static const char* on[] =
    { "NONE", "STR", "STR_FAST", "STR_FAST_STEP_FORWARD", "MAP" };

  fprintf(f, "optimize: %s\n", on[reg->optimize]);
  fprintf(f, "  anchor: "); print_anchor(f, reg->anchor);
  if ((reg->anchor & ANCR_END_BUF_MASK) != 0)
    print_distance_range(f, reg->anc_dist_min, reg->anc_dist_max);
  fprintf(f, "\n");

  if (reg->optimize) {
    fprintf(f, "  sub anchor: "); print_anchor(f, reg->sub_anchor);
    fprintf(f, "\n");
  }
  fprintf(f, "\n");

  if (reg->exact) {
    UChar *p;
    fprintf(f, "exact: [");
    for (p = reg->exact; p < reg->exact_end; p++) {
      fputc(*p, f);
    }
    fprintf(f, "]: length: %ld, dmin: %u, ",
            (reg->exact_end - reg->exact), reg->dist_min);
    if (reg->dist_max == INFINITE_LEN)
      fprintf(f, "dmax: inf.\n");
    else
      fprintf(f, "dmax: %u\n", reg->dist_max);
  }
  else if (reg->optimize & OPTIMIZE_MAP) {
    int c, i, n = 0;

    for (i = 0; i < CHAR_MAP_SIZE; i++)
      if (reg->map[i]) n++;

    fprintf(f, "map: n=%d, dmin: %u, dmax: %u\n",
            n, reg->dist_min, reg->dist_max);
    if (n > 0) {
      c = 0;
      fputc('[', f);
      for (i = 0; i < CHAR_MAP_SIZE; i++) {
        if (reg->map[i] != 0) {
          if (c > 0)  fputs(", ", f);
          c++;
          if (ONIGENC_MBC_MAXLEN(reg->enc) == 1 &&
              ONIGENC_IS_CODE_PRINT(reg->enc, (OnigCodePoint )i))
            fputc(i, f);
          else
            fprintf(f, "%d", i);
        }
      }
      fprintf(f, "]\n");
    }
  }
}