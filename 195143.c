strupper (char *s)
{
while (*s)
  {
   size_t skip = skip_multibyte_char (*s);
   if (skip != 0)
     s += skip;
   else
     {
       if (islower ((uschar)(*s)))
	 *s = toupper (*s);
       s++;
     }
  }
}