sec_generate_keys(uint8 * client_random, uint8 * server_random, int rc4_key_size)
{
	uint8 pre_master_secret[48];
	uint8 master_secret[48];
	uint8 key_block[48];

	/* Construct pre-master secret */
	memcpy(pre_master_secret, client_random, 24);
	memcpy(pre_master_secret + 24, server_random, 24);

	/* Generate master secret and then key material */
	sec_hash_48(master_secret, pre_master_secret, client_random, server_random, 'A');
	sec_hash_48(key_block, master_secret, client_random, server_random, 'X');

	/* First 16 bytes of key material is MAC secret */
	memcpy(g_sec_sign_key, key_block, 16);

	/* Generate export keys from next two blocks of 16 bytes */
	sec_hash_16(g_sec_decrypt_key, &key_block[16], client_random, server_random);
	sec_hash_16(g_sec_encrypt_key, &key_block[32], client_random, server_random);

	if (rc4_key_size == 1)
	{
		DEBUG(("40-bit encryption enabled\n"));
		sec_make_40bit(g_sec_sign_key);
		sec_make_40bit(g_sec_decrypt_key);
		sec_make_40bit(g_sec_encrypt_key);
		g_rc4_key_len = 8;
	}
	else
	{
		DEBUG(("rc_4_key_size == %d, 128-bit encryption enabled\n", rc4_key_size));
		g_rc4_key_len = 16;
	}

	/* Save initial RC4 keys as update keys */
	memcpy(g_sec_decrypt_update_key, g_sec_decrypt_key, 16);
	memcpy(g_sec_encrypt_update_key, g_sec_encrypt_key, 16);

	/* Initialise RC4 state arrays */
	rdssl_rc4_set_key(&g_rc4_decrypt_key, g_sec_decrypt_key, g_rc4_key_len);
	rdssl_rc4_set_key(&g_rc4_encrypt_key, g_sec_encrypt_key, g_rc4_key_len);
}