nf_tables_afinfo_lookup(struct net *net, int family, bool autoload)
{
	struct nft_af_info *afi;

	afi = nft_afinfo_lookup(net, family);
	if (afi != NULL)
		return afi;
#ifdef CONFIG_MODULES
	if (autoload) {
		nfnl_unlock(NFNL_SUBSYS_NFTABLES);
		request_module("nft-afinfo-%u", family);
		nfnl_lock(NFNL_SUBSYS_NFTABLES);
		afi = nft_afinfo_lookup(net, family);
		if (afi != NULL)
			return ERR_PTR(-EAGAIN);
	}
#endif
	return ERR_PTR(-EAFNOSUPPORT);
}