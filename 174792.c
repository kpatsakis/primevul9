calc_hmac_md5 (unsigned char *hmac, const guchar *key, gsize key_sz, const guchar *data, gsize data_sz)
{
	char *hmac_hex, *hex_pos;
	size_t count;

	hmac_hex = g_compute_hmac_for_data(G_CHECKSUM_MD5, key, key_sz, data, data_sz);
	hex_pos = hmac_hex;
	for (count = 0; count < HMAC_MD5_LENGTH; count++)
	{
		/* The 'hh' sscanf format modifier is C99, so we enable it on
		 * non-Windows or if __USE_MINGW_ANSI_STDIO is enabled or`
		 * if we are building on Visual Studio 2015 or later
		 */
#if !defined (G_OS_WIN32) || (__USE_MINGW_ANSI_STDIO == 1) || (_MSC_VER >= 1900)
		sscanf(hex_pos, "%2hhx", &hmac[count]);
#else
		unsigned int tmp_hmac;
		sscanf(hex_pos, "%2x", &tmp_hmac);
		hmac[count] = (guint8)tmp_hmac;
#endif

		hex_pos += 2;
	}
	g_free(hmac_hex);
}