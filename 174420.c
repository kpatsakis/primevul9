sec_sign(uint8 * signature, int siglen, uint8 * session_key, int keylen, uint8 * data, int datalen)
{
	uint8 shasig[20];
	uint8 md5sig[16];
	uint8 lenhdr[4];
	RDSSL_SHA1 sha1;
	RDSSL_MD5 md5;

	buf_out_uint32(lenhdr, datalen);

	rdssl_sha1_init(&sha1);
	rdssl_sha1_update(&sha1, session_key, keylen);
	rdssl_sha1_update(&sha1, pad_54, 40);
	rdssl_sha1_update(&sha1, lenhdr, 4);
	rdssl_sha1_update(&sha1, data, datalen);
	rdssl_sha1_final(&sha1, shasig);

	rdssl_md5_init(&md5);
	rdssl_md5_update(&md5, session_key, keylen);
	rdssl_md5_update(&md5, pad_92, 48);
	rdssl_md5_update(&md5, shasig, 20);
	rdssl_md5_final(&md5, md5sig);

	memcpy(signature, md5sig, siglen);
}