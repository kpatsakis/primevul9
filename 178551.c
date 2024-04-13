cgiGetCookie(const char *name)		/* I - Name of cookie */
{
  return (cupsGetOption(name, num_cookies, cookies));
}