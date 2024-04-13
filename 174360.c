sec_hash_16(uint8 * out, uint8 * in, uint8 * salt1, uint8 * salt2)
{
	RDSSL_MD5 md5;

	rdssl_md5_init(&md5);
	rdssl_md5_update(&md5, in, 16);
	rdssl_md5_update(&md5, salt1, 32);
	rdssl_md5_update(&md5, salt2, 32);
	rdssl_md5_final(&md5, out);
}