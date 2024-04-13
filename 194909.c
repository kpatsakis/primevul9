nf_tables_chain_type_lookup(const struct nft_af_info *afi,
			    const struct nlattr *nla,
			    bool autoload)
{
	const struct nf_chain_type *type;

	type = __nf_tables_chain_type_lookup(afi->family, nla);
	if (type != NULL)
		return type;
#ifdef CONFIG_MODULES
	if (autoload) {
		nfnl_unlock(NFNL_SUBSYS_NFTABLES);
		request_module("nft-chain-%u-%.*s", afi->family,
			       nla_len(nla), (const char *)nla_data(nla));
		nfnl_lock(NFNL_SUBSYS_NFTABLES);
		type = __nf_tables_chain_type_lookup(afi->family, nla);
		if (type != NULL)
			return ERR_PTR(-EAGAIN);
	}
#endif
	return ERR_PTR(-ENOENT);
}