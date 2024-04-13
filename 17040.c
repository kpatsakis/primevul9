static int nft_set_ext_memcpy(const struct nft_set_ext_tmpl *tmpl, u8 id,
			      void *to, const void *from, u32 len)
{
	if (nft_set_ext_check(tmpl, id, len) < 0)
		return -1;

	memcpy(to, from, len);

	return 0;
}