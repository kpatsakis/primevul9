static int calc_hmac(u8 *digest, const u8 *key, unsigned int keylen,
		     const u8 *buf, unsigned int buflen)
{
	struct sdesc *sdesc;
	int ret;

	sdesc = alloc_sdesc(hmacalg);
	if (IS_ERR(sdesc)) {
		pr_info("encrypted_key: can't alloc %s\n", hmac_alg);
		return PTR_ERR(sdesc);
	}

	ret = crypto_shash_setkey(hmacalg, key, keylen);
	if (!ret)
		ret = crypto_shash_digest(&sdesc->shash, buf, buflen, digest);
	kfree(sdesc);
	return ret;
}