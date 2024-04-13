hash_data(const unsigned char *data, size_t datalen, unsigned char *hash, unsigned int mechanismType)
{

	if ((NULL == data) || (NULL == hash))
		return SC_ERROR_INVALID_ARGUMENTS;

	if(mechanismType & SC_ALGORITHM_ECDSA_HASH_SHA1)
	{
		unsigned char data_hash[24] = { 0 };
		size_t len = 0;

		sha1_digest(data, datalen, data_hash);
		len = REVERSE_ORDER4(datalen);
		memcpy(&data_hash[20], &len, 4);
		memcpy(hash, data_hash, 24);
	}
	else if(mechanismType & SC_ALGORITHM_ECDSA_HASH_SHA256)
	{
		unsigned char data_hash[36] = { 0 };
		size_t len = 0;

		sha256_digest(data, datalen, data_hash);
		len = REVERSE_ORDER4(datalen);
		memcpy(&data_hash[32], &len, 4);
		memcpy(hash, data_hash, 36);
	}
	else
	{
		return SC_ERROR_NOT_SUPPORTED;
	}

	return SC_SUCCESS;
}