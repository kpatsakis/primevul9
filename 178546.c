cgiCheckVariables(const char *names)	/* I - Variables to look for */
{
  char		name[255],		/* Current variable name */
		*s;			/* Pointer in string */
  const char	*val;			/* Value of variable */
  int		element;		/* Array element number */


  if (names == NULL)
    return (1);

  while (*names != '\0')
  {
    while (*names == ' ' || *names == ',')
      names ++;

    for (s = name; *names != '\0' && *names != ' ' && *names != ','; s ++, names ++)
      *s = *names;

    *s = 0;
    if (name[0] == '\0')
      break;

    if ((s = strrchr(name, '-')) != NULL)
    {
      *s      = '\0';
      element = atoi(s + 1) - 1;
      val     = cgiGetArray(name, element);
    }
    else
      val = cgiGetVariable(name);

    if (val == NULL)
      return (0);

    if (*val == '\0')
      return (0);	/* Can't be blank, either! */
  }

  return (1);
}