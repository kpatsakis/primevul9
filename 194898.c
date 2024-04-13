static struct nft_table *nf_tables_table_lookup(const struct nft_af_info *afi,
						const struct nlattr *nla)
{
	struct nft_table *table;

	if (nla == NULL)
		return ERR_PTR(-EINVAL);

	table = nft_table_lookup(afi, nla);
	if (table != NULL)
		return table;

	return ERR_PTR(-ENOENT);
}