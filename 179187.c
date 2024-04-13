static unsigned int PNMInteger(Image *image,CommentInfo *comment_info,
  const unsigned int base)
{
  int
    c;

  unsigned int
    value;

  /*
    Skip any leading whitespace.
  */
  do
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      return(0);
    if (c == (int) '#')
      c=PNMComment(image,comment_info);
  } while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
  if (base == 2)
    return((unsigned int) (c-(int) '0'));
  /*
    Evaluate number.
  */
  value=0;
  while (isdigit(c) != 0)
  {
    if (value <= (unsigned int) (INT_MAX/10))
      {
        value*=10;
        if (value <= (unsigned int) (INT_MAX-(c-(int) '0')))
          value+=c-(int) '0';
      }
    c=ReadBlobByte(image);
    if (c == EOF)
      return(0);
  }
  if (c == (int) '#')
    c=PNMComment(image,comment_info);
  return(value);
}