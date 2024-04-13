cgiSetCookie(const char *name,		/* I - Name */
             const char *value,		/* I - Value */
             const char *path,		/* I - Path (typically "/") */
	     const char *domain,	/* I - Domain name */
	     time_t     expires,	/* I - Expiration date (0 for session) */
	     int        secure)		/* I - Require SSL */
{
  num_cookies = cupsAddOption(name, value, num_cookies, &cookies);

  printf("Set-Cookie: %s=%s;", name, value);
  if (path)
    printf(" path=%s;", path);
  if (domain)
    printf(" domain=%s;", domain);
  if (expires)
  {
    char	date[256];		/* Date string */

    printf(" expires=%s;", httpGetDateString2(expires, date, sizeof(date)));
  }
  if (secure)
    puts(" httponly; secure;");
  else
    puts(" httponly;");
}