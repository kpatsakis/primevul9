calc_ntlmv2_response (const char *user, const char *domain,
						const guchar *nt_hash, const gsize nt_hash_sz,
						const guchar *nonce,
						const char *target_info, size_t target_info_sz,
						guchar *lm_resp, size_t lm_resp_sz,
						guchar *nt_resp, size_t nt_resp_sz)
{
	const unsigned char blob_signature[] = {0x01,0x01,0x00,0x00};
	const unsigned char blob_reserved[] = {0x00,0x00,0x00,0x00};
	gint64 blob_timestamp;
	unsigned char client_nonce[8];
	const unsigned char blob_unknown[] = {0x00,0x00,0x00,0x00};

	unsigned char ntv2_hash[HMAC_MD5_LENGTH];
	guchar *nonce_blob, *blob, *p_blob;
	unsigned char nonce_blob_hash[HMAC_MD5_LENGTH];
	unsigned char nonce_client_nonce[16], nonce_client_nonce_hash[HMAC_MD5_LENGTH];
	gchar *user_uppercase, *user_domain, *user_domain_conv;
	gsize user_domain_conv_sz;
	size_t blob_sz;
	int i;

	/* create HMAC-MD5 hash of Unicode uppercase username and Unicode domain */
	user_uppercase = g_utf8_strup (user, strlen (user));
	user_domain = g_strconcat (user_uppercase, domain, NULL);
	user_domain_conv = g_convert (user_domain, -1, "UCS-2LE", "UTF-8", NULL, &user_domain_conv_sz, NULL);
	calc_hmac_md5 (ntv2_hash, nt_hash, nt_hash_sz, (const guchar *)user_domain_conv, user_domain_conv_sz);
	g_free (user_uppercase);
	g_free (user_domain);
	g_free (user_domain_conv);

	/* create random client nonce */
	for (i = 0; i < sizeof (client_nonce); i++)
	{
		client_nonce[i] = g_random_int();
	}

	/* create timestamp for blob
	 * LE, 64-bit signed value, number of tenths of a ms since January 1, 1601.*/
	blob_timestamp = GINT64_TO_LE(((unsigned long)time(NULL) + 11644473600) * 10000000);

	/* create blob */
	blob_sz = sizeof (blob_signature) + sizeof (blob_reserved) +
			sizeof (blob_timestamp) + sizeof (client_nonce) +
			sizeof (blob_unknown) + target_info_sz;
	p_blob = blob = g_malloc (blob_sz);
	memset (blob, 0, blob_sz);
	memcpy (p_blob, blob_signature, sizeof (blob_signature));
	memcpy (p_blob += sizeof (blob_signature), blob_reserved, sizeof (blob_reserved));
	memcpy (p_blob += sizeof (blob_reserved), &blob_timestamp, sizeof (blob_timestamp));
	memcpy (p_blob += sizeof (blob_timestamp), client_nonce, sizeof (client_nonce));
	memcpy (p_blob += sizeof (client_nonce), blob_unknown, sizeof (blob_unknown));
	memcpy (p_blob += sizeof (blob_unknown), target_info, target_info_sz);

	/* create HMAC-MD5 hash of concatenated nonce and blob */
	nonce_blob = g_malloc (NTLM_CHALLENGE_NONCE_LENGTH + blob_sz);
	memcpy (nonce_blob, nonce, NTLM_CHALLENGE_NONCE_LENGTH);
	memcpy (nonce_blob + NTLM_CHALLENGE_NONCE_LENGTH, blob, blob_sz);
	calc_hmac_md5 (nonce_blob_hash, (const guchar *)ntv2_hash, (gsize) sizeof (ntv2_hash), (const guchar *) nonce_blob, (gsize) NTLM_CHALLENGE_NONCE_LENGTH + blob_sz);
	g_free (nonce_blob);

	/* create NTv2 response */
	memset (nt_resp, 0, nt_resp_sz);
	memcpy (nt_resp, nonce_blob_hash, sizeof (nonce_blob_hash));
	memcpy (nt_resp + sizeof (nonce_blob_hash), blob, blob_sz);

	g_free (blob);

	/* LMv2
	 * create HMAC-MD5 hash of concatenated nonce and client nonce
	 */
	memcpy (nonce_client_nonce, nonce, NTLM_CHALLENGE_NONCE_LENGTH);
	memcpy (nonce_client_nonce + NTLM_CHALLENGE_NONCE_LENGTH, client_nonce, sizeof (client_nonce));
	calc_hmac_md5 (nonce_client_nonce_hash, (const guchar *) ntv2_hash, (gsize) sizeof (ntv2_hash), (const guchar *) nonce_client_nonce, (gsize) NTLM_CHALLENGE_NONCE_LENGTH + sizeof (client_nonce));

	/* create LMv2 response */
	memset (lm_resp, 0, lm_resp_sz);
	memcpy (lm_resp, nonce_client_nonce_hash, sizeof (nonce_client_nonce_hash));
	memcpy (lm_resp + sizeof (nonce_client_nonce_hash), client_nonce, sizeof (client_nonce));
}