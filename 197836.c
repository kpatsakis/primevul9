includeFile (FileInfo * nested, CharsString * includedFile)
{
  int k;
  char includeThis[MAXSTRING];
  char **tableFiles;
  for (k = 0; k < includedFile->length; k++)
    includeThis[k] = (char) includedFile->chars[k];
  includeThis[k] = 0;
  tableFiles = resolveTable (includeThis, nested->fileName);
  if (tableFiles == NULL)
    {
      errorCount++;
      return 0;
    }
  if (tableFiles[1] != NULL)
    {
      errorCount++;
      lou_logPrint ("Table list not supported in include statement: 'include %s'", includeThis);
      return 0;
    }
  return compileFile (*tableFiles);
}