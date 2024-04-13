sec_process_crypt_info(STREAM s)
{
	uint8 *server_random = NULL;
	uint8 modulus[SEC_MAX_MODULUS_SIZE];
	uint8 exponent[SEC_EXPONENT_SIZE];
	uint32 rc4_key_size;

	memset(modulus, 0, sizeof(modulus));
	memset(exponent, 0, sizeof(exponent));
	if (!sec_parse_crypt_info(s, &rc4_key_size, &server_random, modulus, exponent))
	{
		DEBUG(("Failed to parse crypt info\n"));
		return;
	}
	DEBUG(("Generating client random\n"));
	generate_random(g_client_random);
	sec_rsa_encrypt(g_sec_crypted_random, g_client_random, SEC_RANDOM_SIZE,
			g_server_public_key_len, modulus, exponent);
	sec_generate_keys(g_client_random, server_random, rc4_key_size);
}