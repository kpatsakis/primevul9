lou_setDataPath (char *path)
{
  dataPathPtr = NULL;
  if (path == NULL)
    return NULL;
  strcpy (dataPath, path);
  dataPathPtr = dataPath;
  return dataPathPtr;
}