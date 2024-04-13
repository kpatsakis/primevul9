onig_print_compiled_byte_code_list(FILE* f, regex_t* reg)
{
  Operation* bp;
  Operation* start = reg->ops;
  Operation* end   = reg->ops + reg->ops_used;

  fprintf(f, "bt_mem_start: 0x%x, bt_mem_end: 0x%x\n",
          reg->bt_mem_start, reg->bt_mem_end);
  fprintf(f, "code-length: %d\n", reg->ops_used);

  bp = start;
  while (bp < end) {
    int pos = bp - start;

    fprintf(f, "%4d: ", pos);
    print_compiled_byte_code(f, reg, pos, start, reg->enc);
    fprintf(f, "\n");
    bp++;
  }
  fprintf(f, "\n");
}