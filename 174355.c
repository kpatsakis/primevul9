sec_hash_sha1_16(uint8 * out, uint8 * in, uint8 * salt1)
{
	RDSSL_SHA1 sha1;
	rdssl_sha1_init(&sha1);
	rdssl_sha1_update(&sha1, in, 16);
	rdssl_sha1_update(&sha1, salt1, 16);
	rdssl_sha1_final(&sha1, out);
}