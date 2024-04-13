void *nft_set_elem_init(const struct nft_set *set,
			const struct nft_set_ext_tmpl *tmpl,
			const u32 *key, const u32 *key_end,
			const u32 *data, u64 timeout, u64 expiration, gfp_t gfp)
{
	struct nft_set_ext *ext;
	void *elem;

	elem = kzalloc(set->ops->elemsize + tmpl->len, gfp);
	if (elem == NULL)
		return ERR_PTR(-ENOMEM);

	ext = nft_set_elem_ext(set, elem);
	nft_set_ext_init(ext, tmpl);

	if (nft_set_ext_exists(ext, NFT_SET_EXT_KEY) &&
	    nft_set_ext_memcpy(tmpl, NFT_SET_EXT_KEY,
			       nft_set_ext_key(ext), key, set->klen) < 0)
		goto err_ext_check;

	if (nft_set_ext_exists(ext, NFT_SET_EXT_KEY_END) &&
	    nft_set_ext_memcpy(tmpl, NFT_SET_EXT_KEY_END,
			       nft_set_ext_key_end(ext), key_end, set->klen) < 0)
		goto err_ext_check;

	if (nft_set_ext_exists(ext, NFT_SET_EXT_DATA) &&
	    nft_set_ext_memcpy(tmpl, NFT_SET_EXT_DATA,
			       nft_set_ext_data(ext), data, set->dlen) < 0)
		goto err_ext_check;

	if (nft_set_ext_exists(ext, NFT_SET_EXT_EXPIRATION)) {
		*nft_set_ext_expiration(ext) = get_jiffies_64() + expiration;
		if (expiration == 0)
			*nft_set_ext_expiration(ext) += timeout;
	}
	if (nft_set_ext_exists(ext, NFT_SET_EXT_TIMEOUT))
		*nft_set_ext_timeout(ext) = timeout;

	return elem;

err_ext_check:
	kfree(elem);

	return ERR_PTR(-EINVAL);
}