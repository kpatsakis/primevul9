static struct skcipher_request *init_skcipher_req(const u8 *key,
						  unsigned int key_len)
{
	struct skcipher_request *req;
	struct crypto_skcipher *tfm;
	int ret;

	tfm = crypto_alloc_skcipher(blkcipher_alg, 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(tfm)) {
		pr_err("encrypted_key: failed to load %s transform (%ld)\n",
		       blkcipher_alg, PTR_ERR(tfm));
		return ERR_CAST(tfm);
	}

	ret = crypto_skcipher_setkey(tfm, key, key_len);
	if (ret < 0) {
		pr_err("encrypted_key: failed to setkey (%d)\n", ret);
		crypto_free_skcipher(tfm);
		return ERR_PTR(ret);
	}

	req = skcipher_request_alloc(tfm, GFP_KERNEL);
	if (!req) {
		pr_err("encrypted_key: failed to allocate request for %s\n",
		       blkcipher_alg);
		crypto_free_skcipher(tfm);
		return ERR_PTR(-ENOMEM);
	}

	skcipher_request_set_callback(req, 0, NULL, NULL);
	return req;
}