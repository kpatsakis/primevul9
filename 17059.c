static int nft_set_ext_check(const struct nft_set_ext_tmpl *tmpl, u8 id, u32 len)
{
	len += nft_set_ext_types[id].len;
	if (len > tmpl->ext_len[id] ||
	    len > U8_MAX)
		return -1;

	return 0;
}