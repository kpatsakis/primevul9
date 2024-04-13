lou_compileString (const char *tableList, const char *inString)
{
  if (!lou_getTable (tableList))
    return 0;
  return compileString (inString);
}