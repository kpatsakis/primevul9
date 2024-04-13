lou_readCharFromFile (const char *fileName, int *mode)
{
/*Read a character from a file, whether big-endian, little-endian or 
* ASCII8*/
  int ch;
  static FileInfo nested;
  if (fileName == NULL)
    return 0;
  if (*mode == 1)
    {
      *mode = 0;
      nested.fileName = fileName;
      nested.encoding = noEncoding;
      nested.status = 0;
      nested.lineNumber = 0;
      if (!(nested.in = fopen (nested.fileName, "r")))
	{
	  lou_logPrint ("Cannot open file '%s'", nested.fileName);
	  *mode = 1;
	  return EOF;
	}
    }
  if (nested.in == NULL)
    {
      *mode = 1;
      return EOF;
    }
  ch = getAChar (&nested);
  if (ch == EOF)
    {
      fclose (nested.in);
      nested.in = NULL;
      *mode = 1;
    }
  return ch;
}