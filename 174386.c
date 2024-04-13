sec_hash_48(uint8 * out, uint8 * in, uint8 * salt1, uint8 * salt2, uint8 salt)
{
	uint8 shasig[20];
	uint8 pad[4];
	RDSSL_SHA1 sha1;
	RDSSL_MD5 md5;
	int i;

	for (i = 0; i < 3; i++)
	{
		memset(pad, salt + i, i + 1);

		rdssl_sha1_init(&sha1);
		rdssl_sha1_update(&sha1, pad, i + 1);
		rdssl_sha1_update(&sha1, in, 48);
		rdssl_sha1_update(&sha1, salt1, 32);
		rdssl_sha1_update(&sha1, salt2, 32);
		rdssl_sha1_final(&sha1, shasig);

		rdssl_md5_init(&md5);
		rdssl_md5_update(&md5, in, 48);
		rdssl_md5_update(&md5, shasig, 20);
		rdssl_md5_final(&md5, &out[i * 16]);
	}
}