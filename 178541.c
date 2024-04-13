cgi_initialize_cookies(void)
{
  const char	*cookie;		/* HTTP_COOKIE environment variable */
  char		name[128],		/* Name string */
		value[512],		/* Value string */
		*ptr;			/* Pointer into name/value */


  if ((cookie = getenv("HTTP_COOKIE")) == NULL)
    return;

  while (*cookie)
  {
    int	skip = 0;			/* Skip this cookie? */

   /*
    * Skip leading whitespace...
    */

    while (isspace(*cookie & 255))
      cookie ++;
    if (!*cookie)
      break;

   /*
    * Copy the name...
    */

    for (ptr = name; *cookie && *cookie != '=';)
      if (ptr < (name + sizeof(name) - 1))
      {
        *ptr++ = *cookie++;
      }
      else
      {
        skip = 1;
	cookie ++;
      }

    if (*cookie != '=')
      break;

    *ptr = '\0';
    cookie ++;

   /*
    * Then the value...
    */

    if (*cookie == '\"')
    {
      for (cookie ++, ptr = value; *cookie && *cookie != '\"';)
        if (ptr < (value + sizeof(value) - 1))
	{
	  *ptr++ = *cookie++;
	}
	else
	{
	  skip = 1;
	  cookie ++;
	}

      if (*cookie == '\"')
        cookie ++;
      else
        skip = 1;
    }
    else
    {
      for (ptr = value; *cookie && *cookie != ';';)
        if (ptr < (value + sizeof(value) - 1))
	{
	  *ptr++ = *cookie++;
	}
	else
	{
	  skip = 1;
	  cookie ++;
	}
    }

    if (*cookie == ';')
      cookie ++;
    else if (*cookie)
      skip = 1;

    *ptr = '\0';

   /*
    * Then add the cookie to an array as long as the name doesn't start with
    * "$"...
    */

    if (name[0] != '$' && !skip)
      num_cookies = cupsAddOption(name, value, num_cookies, &cookies);
  }
}