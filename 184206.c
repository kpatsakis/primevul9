set_addr_in_repeat_range(regex_t* reg)
{
  int i;

  for (i = 0; i < reg->num_repeat; i++) {
    RepeatRange* p = reg->repeat_range + i;
    int offset = p->u.offset;
    p->u.pcode = reg->ops + offset;
  }
}