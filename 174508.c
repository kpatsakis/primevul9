sec_parse_public_sig(STREAM s, uint32 len, uint8 * modulus, uint8 * exponent)
{
	uint8 signature[SEC_MAX_MODULUS_SIZE];
	uint32 sig_len;

	if (len != 72)
	{
		return True;
	}
	memset(signature, 0, sizeof(signature));
	sig_len = len - 8;
	in_uint8a(s, signature, sig_len);
	return rdssl_sig_ok(exponent, SEC_EXPONENT_SIZE, modulus, g_server_public_key_len,
			    signature, sig_len);
}