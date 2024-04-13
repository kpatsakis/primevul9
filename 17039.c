static bool nft_setelem_valid_key_end(const struct nft_set *set,
				      struct nlattr **nla, u32 flags)
{
	if ((set->flags & (NFT_SET_CONCAT | NFT_SET_INTERVAL)) ==
			  (NFT_SET_CONCAT | NFT_SET_INTERVAL)) {
		if (flags & NFT_SET_ELEM_INTERVAL_END)
			return false;
		if (!nla[NFTA_SET_ELEM_KEY_END] &&
		    !(flags & NFT_SET_ELEM_CATCHALL))
			return false;
	} else {
		if (nla[NFTA_SET_ELEM_KEY_END])
			return false;
	}

	return true;
}