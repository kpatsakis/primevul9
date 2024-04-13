getToken (FileInfo * nested, CharsString * result, const char *description)
{
/*Find the next string of contiguous non-whitespace characters. If this 
 * is the last token on the line, return 2 instead of 1. */
  while (nested->line[nested->linepos] && nested->line[nested->linepos] <= 32)
    nested->linepos++;
  result->length = 0;
  while (nested->line[nested->linepos] && nested->line[nested->linepos] > 32)
    {
    int maxlen = MAXSTRING;
    if (result->length >= maxlen)
    {
    compileError (nested, "more than %d characters (bytes)", maxlen);
    return 0;
    }
    else
    result->chars[result->length++] = nested->line[nested->linepos++];
    }
  if (!result->length)
    {
      /* Not enough tokens */
      if (description)
	compileError (nested, "%s not specified.", description);
      return 0;
    }
  result->chars[result->length] = 0;
  while (nested->line[nested->linepos] && nested->line[nested->linepos] <= 32)
    nested->linepos++;
  if (nested->line[nested->linepos] == 0)
    {
      lastToken = 1;
      return 2;
    }
  else
    {
      lastToken = 0;
      return 1;
    }
}