cgi_passwd(const char *prompt)		/* I - Prompt (not used) */
{
  (void)prompt;

  fprintf(stderr, "DEBUG: cgi_passwd(prompt=\"%s\") called!\n",
          prompt ? prompt : "(null)");

 /*
  * Send a 401 (unauthorized) status to the server, so it can notify
  * the client that authentication is required.
  */

  puts("Status: 401\n");
  exit(0);

 /*
  * This code is never executed, but is present to satisfy the compiler.
  */

  return (NULL);
}