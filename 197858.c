resolveSubtable (const char *table, const char *base, const char *searchPath)
{
  char *tableFile;
  static struct stat info;
  
  if (table == NULL || table[0] == '\0')
    return NULL;
  tableFile = (char *) malloc (MAXSTRING * sizeof(char));
  
  //
  // First try to resolve against base
  //
  if (base)
    {
      int k;
      strcpy (tableFile, base);
      for (k = strlen (tableFile); k >= 0 && tableFile[k] != DIR_SEP; k--)
	;
      tableFile[++k] = '\0';
      strcat (tableFile, table);
      if (stat (tableFile, &info) == 0 && !(info.st_mode & S_IFDIR))
	return tableFile;
    }
  
  //
  // It could be an absolute path, or a path relative to the current working
  // directory
  //
  strcpy (tableFile, table);
  if (stat (tableFile, &info) == 0 && !(info.st_mode & S_IFDIR))
    return tableFile;
  
  //
  // Then search `LOUIS_TABLEPATH`, `dataPath` and `programPath`
  //
  if (searchPath[0] != '\0')
    {
      char *dir;
      int last;
      char *cp;
      for (dir = strdup (searchPath + 1); ; dir = cp + 1)
	{
	  for (cp = dir; *cp != '\0' && *cp != ','; cp++)
	    ;
	  last = (*cp == '\0');
	  *cp = '\0';
	  if (dir == cp)
	    dir = ".";
	  sprintf (tableFile, "%s%c%s", dir, DIR_SEP, table);
	  if (stat (tableFile, &info) == 0 && !(info.st_mode & S_IFDIR))
	    return tableFile;
	  if (last)
	    break;
	}
    }
  free (tableFile);
  return NULL;
}