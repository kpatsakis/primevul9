soup_ntlm_lanmanager_hash (const char *password, guchar hash[21])
{
	guchar lm_password [15];
	DES_KS ks;
	int i;

	for (i = 0; i < 14 && password [i]; i++)
		lm_password [i] = g_ascii_toupper ((unsigned char) password [i]);

	for (; i < 15; i++)
		lm_password [i] = '\0';

	memcpy (hash, LM_PASSWORD_MAGIC, 21);

	setup_schedule (lm_password, ks);
	des (ks, hash);

	setup_schedule (lm_password + 7, ks);
	des (ks, hash + 8);
}