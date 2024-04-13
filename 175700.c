static int __init encrypted_shash_alloc(void)
{
	int ret;

	hmacalg = crypto_alloc_shash(hmac_alg, 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(hmacalg)) {
		pr_info("encrypted_key: could not allocate crypto %s\n",
			hmac_alg);
		return PTR_ERR(hmacalg);
	}

	hashalg = crypto_alloc_shash(hash_alg, 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(hashalg)) {
		pr_info("encrypted_key: could not allocate crypto %s\n",
			hash_alg);
		ret = PTR_ERR(hashalg);
		goto hashalg_fail;
	}

	return 0;

hashalg_fail:
	crypto_free_shash(hmacalg);
	return ret;
}