static int reqs_reencrypt_online(uint32_t reqs)
{
	return reqs & CRYPT_REQUIREMENT_ONLINE_REENCRYPT;
}