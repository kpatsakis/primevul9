cgi_initialize_string(const char *data)	/* I - Form data string */
{
  int	done;				/* True if we're done reading a form variable */
  char	*s,				/* Pointer to current form string */
	ch,				/* Temporary character */
	name[255],			/* Name of form variable */
	value[65536];			/* Variable value */


 /*
  * Check input...
  */

  if (data == NULL)
    return (0);

 /*
  * Loop until we've read all the form data...
  */

  while (*data != '\0')
  {
   /*
    * Get the variable name...
    */

    for (s = name; *data != '\0'; data ++)
      if (*data == '=')
        break;
      else if (*data >= ' ' && s < (name + sizeof(name) - 1))
        *s++ = *data;

    *s = '\0';
    if (*data == '=')
      data ++;
    else
      return (0);

   /*
    * Read the variable value...
    */

    for (s = value, done = 0; !done && *data != '\0'; data ++)
      switch (*data)
      {
	case '&' :	/* End of data... */
            done = 1;
            break;

	case '+' :	/* Escaped space character */
            if (s < (value + sizeof(value) - 1))
              *s++ = ' ';
            break;

	case '%' :	/* Escaped control character */
	   /*
	    * Read the hex code...
	    */

            if (!isxdigit(data[1] & 255) || !isxdigit(data[2] & 255))
	      return (0);

            if (s < (value + sizeof(value) - 1))
	    {
              data ++;
              ch = *data - '0';
              if (ch > 9)
        	ch -= 7;
              *s = (char)(ch << 4);

              data ++;
              ch = *data - '0';
              if (ch > 9)
        	ch -= 7;
              *s++ |= ch;
            }
	    else
	      data += 2;
            break;

	default :	/* Other characters come straight through */
	    if (*data >= ' ' && s < (value + sizeof(value) - 1))
              *s++ = *data;
            break;
      }

    *s = '\0';		/* nul terminate the string */

   /*
    * Remove trailing whitespace...
    */

    if (s > value)
      s --;

    while (s >= value && isspace(*s & 255))
      *s-- = '\0';

   /*
    * Add the string to the variable "database"...
    */

    if ((s = strrchr(name, '-')) != NULL && isdigit(s[1] & 255))
    {
      *s++ = '\0';
      if (value[0])
        cgiSetArray(name, atoi(s) - 1, value);
    }
    else if (cgiGetVariable(name) != NULL)
      cgiSetArray(name, cgiGetSize(name), value);
    else
      cgiSetVariable(name, value);
  }

  return (1);
}