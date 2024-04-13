static inline MagickBooleanType IsPoint(const char *point)
{
  char
    *p;

  long
    value;

  value=strtol(point,&p,10);
  (void) value;
  return(p != point ? MagickTrue : MagickFalse);
}