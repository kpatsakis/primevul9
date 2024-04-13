sec_rsa_encrypt(uint8 * out, uint8 * in, int len, uint32 modulus_size, uint8 * modulus,
		uint8 * exponent)
{
	rdssl_rsa_encrypt(out, in, len, modulus_size, modulus, exponent);
}