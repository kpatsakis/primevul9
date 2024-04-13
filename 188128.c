static int DelegateCompare(const void *x,const void *y)
{
  register const char
    **p,
    **q;

  p=(const char **) x;
  q=(const char **) y;
  return(LocaleCompare(*p,*q));
}