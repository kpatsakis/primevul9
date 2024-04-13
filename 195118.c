spa_build_auth_response (SPAAuthChallenge * challenge,
                        SPAAuthResponse * response, char *user,
                        char *password)
{
uint8x lmRespData[24];
uint8x ntRespData[24];
char *d = strdup (GetUnicodeString (challenge, uDomain));
char *domain = d;
char *u = strdup (user);
char *p = strchr (u, '@');

if (p)
  {
  domain = p + 1;
  *p = '\0';
  }

spa_smb_encrypt (US password, challenge->challengeData, lmRespData);
spa_smb_nt_encrypt (US password, challenge->challengeData, ntRespData);

response->bufIndex = 0;
memcpy (response->ident, "NTLMSSP\0\0\0", 8);
SIVAL (&response->msgType, 0, 3);

spa_bytes_add (response, lmResponse, lmRespData, 24);
spa_bytes_add (response, ntResponse, ntRespData, 24);
spa_unicode_add_string (response, uDomain, domain);
spa_unicode_add_string (response, uUser, u);
spa_unicode_add_string (response, uWks, u);
spa_string_add (response, sessionKey, NULL);

response->flags = challenge->flags;

free (d);
free (u);
}