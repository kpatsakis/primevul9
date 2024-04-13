nt_lm_owf_gen (char *pwd, uschar nt_p16[16], uschar p16[16])
{
char passwd[130];

memset (passwd, '\0', 130);
safe_strcpy (passwd, pwd, sizeof (passwd) - 1);

/* Calculate the MD4 hash (NT compatible) of the password */
memset (nt_p16, '\0', 16);
E_md4hash (US passwd, nt_p16);

#ifdef DEBUG_PASSWORD
DEBUG_X (100, ("nt_lm_owf_gen: pwd, nt#\n"));
dump_data (120, passwd, strlen (passwd));
dump_data (100, CS  nt_p16, 16);
#endif

/* Mangle the passwords into Lanman format */
passwd[14] = '\0';
strupper (passwd);

/* Calculate the SMB (lanman) hash functions of the password */

memset (p16, '\0', 16);
E_P16 (US passwd, US p16);

#ifdef DEBUG_PASSWORD
DEBUG_X (100, ("nt_lm_owf_gen: pwd, lm#\n"));
dump_data (120, passwd, strlen (passwd));
dump_data (100, CS  p16, 16);
#endif
/* clear out local copy of user's password (just being paranoid). */
memset (passwd, '\0', sizeof (passwd));
}