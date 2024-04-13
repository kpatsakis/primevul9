static int nft_setelem_parse_flags(const struct nft_set *set,
				   const struct nlattr *attr, u32 *flags)
{
	if (attr == NULL)
		return 0;

	*flags = ntohl(nla_get_be32(attr));
	if (*flags & ~(NFT_SET_ELEM_INTERVAL_END | NFT_SET_ELEM_CATCHALL))
		return -EOPNOTSUPP;
	if (!(set->flags & NFT_SET_INTERVAL) &&
	    *flags & NFT_SET_ELEM_INTERVAL_END)
		return -EINVAL;
	if ((*flags & (NFT_SET_ELEM_INTERVAL_END | NFT_SET_ELEM_CATCHALL)) ==
	    (NFT_SET_ELEM_INTERVAL_END | NFT_SET_ELEM_CATCHALL))
		return -EINVAL;

	return 0;
}