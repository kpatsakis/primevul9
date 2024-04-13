path_is_similar (const char *path1, const char *path2)
{
  int i;

  for (i = 0; path1[i]; i++)
    {
      if (path2[i] == '\0')
        return FALSE;

      if (tolower (path1[i]) == tolower (path2[i]))
        continue;

      if ((path1[i] == '-' || path1[i] == '_') &&
          (path2[i] == '-' || path2[i] == '_'))
        continue;

      return FALSE;
    }

  if (path2[0] != '\0')
    return FALSE;

  return TRUE;
}