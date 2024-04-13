compileString (const char *inString)
{
/* This function can be used to make changes to tables on the fly. */
  int k;
  FileInfo nested;
  if (inString == NULL)
    return 0;
  nested.fileName = inString;
  nested.encoding = noEncoding;
  nested.lineNumber = 1;
  nested.status = 0;
  nested.linepos = 0;
  for (k = 0; inString[k]; k++)
    nested.line[k] = inString[k];
  nested.line[k] = 0;
  return compileRule (&nested);
}