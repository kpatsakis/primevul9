getALine (FileInfo * nested)
{
/*Read a line of widechar's from an input file */
  int ch;
  int pch = 0;
  nested->linelen = 0;
  while ((ch = getAChar (nested)) != EOF)
    {
      if (ch == 13)
	continue;
      if (pch == '\\' && ch == 10)
	{
	  nested->linelen--;
	  continue;
	}
      if (ch == 10 || nested->linelen >= MAXSTRING)
	break;
      nested->line[nested->linelen++] = (widechar) ch;
      pch = ch;
    }
  nested->line[nested->linelen] = 0;
  nested->linepos = 0;
  if (ch == EOF)
    return 0;
  nested->lineNumber++;
  return 1;
}