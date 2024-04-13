charHash (widechar c)
{
  unsigned long int makeHash = (unsigned long int) c % HASHNUM;
  return (int) makeHash;
}