int tm_notify(int tm_signal)
  {
  if (!init_done)
    return TM_BADINIT;

  return TM_ENOTIMPLEMENTED;
  }