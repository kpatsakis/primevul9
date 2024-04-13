struct nft_set *nf_tables_set_lookup(const struct nft_table *table,
				     const struct nlattr *nla)
{
	struct nft_set *set;

	if (nla == NULL)
		return ERR_PTR(-EINVAL);

	list_for_each_entry(set, &table->sets, list) {
		if (!nla_strcmp(nla, set->name))
			return set;
	}
	return ERR_PTR(-ENOENT);
}