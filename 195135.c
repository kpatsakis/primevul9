spa_build_auth_request (SPAAuthRequest * request, char *user, char *domain)
{
char *u = strdup (user);
char *p = strchr (u, '@');

if (p)
  {
  if (!domain)
   domain = p + 1;
  *p = '\0';
  }

request->bufIndex = 0;
memcpy (request->ident, "NTLMSSP\0\0\0", 8);
SIVAL (&request->msgType, 0, 1);
SIVAL (&request->flags, 0, 0x0000b207);      /* have to figure out what these mean */
spa_string_add (request, user, u);
spa_string_add (request, domain, domain);
free (u);
}