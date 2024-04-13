is_left(int a)
{
  if (a == ANCR_END_BUF  || a == ANCR_SEMI_END_BUF ||
      a == ANCR_END_LINE || a == ANCR_PREC_READ || a == ANCR_PREC_READ_NOT)
    return 0;

  return 1;
}