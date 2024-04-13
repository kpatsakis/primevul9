defaultTableResolver (const char *tableList, const char *base)
{
  char searchPath[MAXSTRING];
  char **tableFiles;
  char *subTable;
  char *cp;
  char *path;
  int last;
  int k;
  
  /* Set up search path */
  cp = searchPath;
  path = getenv ("LOUIS_TABLEPATH");
  if (path != NULL && path[0] != '\0')
    cp += sprintf (cp, ",%s", path);
  path = lou_getDataPath ();
  if (path != NULL && path[0] != '\0')
    cp += sprintf (cp, ",%s%c%s%c%s", path, DIR_SEP, "liblouis", DIR_SEP,
		   "tables");
#ifdef _WIN32
  path = lou_getProgramPath ();
  if (path != NULL && path[0] != '\0')
    cp += sprintf (cp, ",%s%s", path, "\\share\\liblouis\\tables");
#else
  cp += sprintf (cp, ",%s", TABLESDIR);
#endif
  
  /* Count number of subtables in table list */
  k = 0;
  for (cp = tableList; *cp != '\0'; cp++)
    if (*cp == ',')
      k++;
  tableFiles = (char **) malloc ((k + 2) * sizeof(char *));
  
  /* Resolve subtables */
  k = 0;
  for (subTable = strdup (tableList); ; subTable = cp + 1)
    {
      for (cp = subTable; *cp != '\0' && *cp != ','; cp++);
      last = (*cp == '\0');
      *cp = '\0';
      if (!(tableFiles[k++] = resolveSubtable (subTable, base, searchPath)))
	{
	  lou_logPrint ("Cannot resolve table '%s'", subTable);
	  free (tableFiles);
	  return NULL;
	}
      if (k == 1)
	base = subTable;
      if (last)
	break;
    }
  tableFiles[k] = NULL;
  return tableFiles;
}