static int calc_hash(u8 *digest, const u8 *buf, unsigned int buflen)
{
	struct sdesc *sdesc;
	int ret;

	sdesc = alloc_sdesc(hashalg);
	if (IS_ERR(sdesc)) {
		pr_info("encrypted_key: can't alloc %s\n", hash_alg);
		return PTR_ERR(sdesc);
	}

	ret = crypto_shash_digest(&sdesc->shash, buf, buflen, digest);
	kfree(sdesc);
	return ret;
}