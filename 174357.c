sec_update(uint8 * key, uint8 * update_key)
{
	uint8 shasig[20];
	RDSSL_SHA1 sha1;
	RDSSL_MD5 md5;
	RDSSL_RC4 update;

	rdssl_sha1_init(&sha1);
	rdssl_sha1_update(&sha1, update_key, g_rc4_key_len);
	rdssl_sha1_update(&sha1, pad_54, 40);
	rdssl_sha1_update(&sha1, key, g_rc4_key_len);
	rdssl_sha1_final(&sha1, shasig);

	rdssl_md5_init(&md5);
	rdssl_md5_update(&md5, update_key, g_rc4_key_len);
	rdssl_md5_update(&md5, pad_92, 48);
	rdssl_md5_update(&md5, shasig, 20);
	rdssl_md5_final(&md5, key);

	rdssl_rc4_set_key(&update, key, g_rc4_key_len);
	rdssl_rc4_crypt(&update, key, key, g_rc4_key_len);

	if (g_rc4_key_len == 8)
		sec_make_40bit(key);
}