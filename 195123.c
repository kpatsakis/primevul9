spa_build_auth_response (SPAAuthChallenge * challenge,
                        SPAAuthResponse * response, char *user,
                        char *password)
{
uint8x lmRespData[24];
uint8x ntRespData[24];
uint32x cf = IVAL(&challenge->flags, 0);
char *u = strdup (user);
char *p = strchr (u, '@');
char *d = NULL;
char *domain;

if (p)
  {
  domain = p + 1;
  *p = '\0';
  }

else domain = d = strdup((cf & 0x1)?
  CCS GetUnicodeString(challenge, uDomain) :
  CCS GetString(challenge, uDomain));

spa_smb_encrypt (US password, challenge->challengeData, lmRespData);
spa_smb_nt_encrypt (US password, challenge->challengeData, ntRespData);

response->bufIndex = 0;
memcpy (response->ident, "NTLMSSP\0\0\0", 8);
SIVAL (&response->msgType, 0, 3);

spa_bytes_add (response, lmResponse, lmRespData, (cf & 0x200) ? 24 : 0);
spa_bytes_add (response, ntResponse, ntRespData, (cf & 0x8000) ? 24 : 0);

if (cf & 0x1) {      /* Unicode Text */
     spa_unicode_add_string (response, uDomain, domain);
     spa_unicode_add_string (response, uUser, u);
     spa_unicode_add_string (response, uWks, u);
} else {             /* OEM Text */
     spa_string_add (response, uDomain, domain);
     spa_string_add (response, uUser, u);
     spa_string_add (response, uWks, u);
}

spa_string_add (response, sessionKey, NULL);
response->flags = challenge->flags;

if (d != NULL) free (d);
free (u);
}