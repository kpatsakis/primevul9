segment_holding_code (mstate m, char* addr)
{
  msegmentptr sp = &m->seg;
  for (;;) {
    if (addr >= add_segment_exec_offset (sp->base, sp)
	&& addr < add_segment_exec_offset (sp->base, sp) + sp->size)
      return sp;
    if ((sp = sp->next) == 0)
      return 0;
  }
}