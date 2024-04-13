sec_encrypt(uint8 * data, int length)
{
	if (g_sec_encrypt_use_count == 4096)
	{
		sec_update(g_sec_encrypt_key, g_sec_encrypt_update_key);
		rdssl_rc4_set_key(&g_rc4_encrypt_key, g_sec_encrypt_key, g_rc4_key_len);
		g_sec_encrypt_use_count = 0;
	}

	rdssl_rc4_crypt(&g_rc4_encrypt_key, data, data, length);
	g_sec_encrypt_use_count++;
}