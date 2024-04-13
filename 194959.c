static struct nft_chain *nf_tables_chain_lookup(const struct nft_table *table,
						const struct nlattr *nla)
{
	struct nft_chain *chain;

	if (nla == NULL)
		return ERR_PTR(-EINVAL);

	list_for_each_entry(chain, &table->chains, list) {
		if (!nla_strcmp(nla, chain->name))
			return chain;
	}

	return ERR_PTR(-ENOENT);
}