static int reqs_reencrypt(uint32_t reqs)
{
	return reqs & CRYPT_REQUIREMENT_OFFLINE_REENCRYPT;
}