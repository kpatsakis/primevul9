cgi_initialize_get(void)
{
  char	*data;				/* Pointer to form data string */


 /*
  * Check to see if there is anything for us to read...
  */

  data = getenv("QUERY_STRING");
  if (data == NULL || strlen(data) == 0)
    return (0);

 /*
  * Parse it out and return...
  */

  return (cgi_initialize_string(data));
}