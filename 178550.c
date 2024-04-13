cgiIsPOST(void)
{
  const char	*method;		/* REQUEST_METHOD environment variable */


  if ((method = getenv("REQUEST_METHOD")) == NULL)
    return (0);
  else
    return (!strcmp(method, "POST"));
}