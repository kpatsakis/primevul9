rar_fls(unsigned int word)
{
  word |= (word >>  1);
  word |= (word >>  2);
  word |= (word >>  4);
  word |= (word >>  8);
  word |= (word >> 16);
  return word - (word >> 1);
}