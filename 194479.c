static int LogInfoCompare(const void *x,const void *y)
{
  const LogInfo
    **p,
    **q;

  p=(const LogInfo **) x,
  q=(const LogInfo **) y;
  if (LocaleCompare((*p)->path,(*q)->path) == 0)
    return(LocaleCompare((*p)->name,(*q)->name));
  return(LocaleCompare((*p)->path,(*q)->path));
}