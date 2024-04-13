static void encrypted_shash_release(void)
{
	if (hashalg)
		crypto_free_shash(hashalg);
	if (hmacalg)
		crypto_free_shash(hmacalg);
}