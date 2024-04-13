lou_getTable (const char *tableList)
{
  void *table = NULL;
  if (tableList == NULL || tableList[0] == 0)
    return NULL;
  errorCount = fileCount = 0;
  table = getTable (tableList);
  if (!table)
    lou_logPrint ("%s could not be found", tableList);
  return table;
}