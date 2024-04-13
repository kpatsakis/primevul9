stringHash (const widechar * c)
{
/*hash function for strings */
  unsigned long int makeHash = (((unsigned long int) c[0] << 8) +
				(unsigned long int) c[1]) % HASHNUM;
  return (int) makeHash;
}