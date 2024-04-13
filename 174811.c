calc_ntlm2_session_response (const char *nonce,
			     guchar      nt_hash[21],
			     guchar      lm_hash[21],
			     guchar     *lm_resp,
			     gsize       lm_resp_sz,
			     guchar     *nt_resp)
{
	guint32 client_nonce[2];
	guchar ntlmv2_hash[16];
	GChecksum *ntlmv2_cksum;
	gsize ntlmv2_hash_sz = sizeof (ntlmv2_hash);

	/* FIXME: if GLib ever gets a more secure random number
	 * generator, use it here
	 */
	client_nonce[0] = g_random_int();
	client_nonce[1] = g_random_int();

	ntlmv2_cksum = g_checksum_new (G_CHECKSUM_MD5);
	g_checksum_update (ntlmv2_cksum, (const guchar *) nonce, 8);
	g_checksum_update (ntlmv2_cksum, (const guchar *) client_nonce, sizeof (client_nonce));
	g_checksum_get_digest (ntlmv2_cksum, ntlmv2_hash, &ntlmv2_hash_sz);
	g_checksum_free (ntlmv2_cksum);

	/* Send the padded client nonce as a fake lm_resp */
	memset (lm_resp, 0, lm_resp_sz);
	memcpy (lm_resp, client_nonce, sizeof (client_nonce));

	/* Compute nt_hash as usual but with a new nonce */
	calc_response (nt_hash, ntlmv2_hash, nt_resp);
}