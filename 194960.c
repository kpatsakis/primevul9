static struct nft_table *nft_table_lookup(const struct nft_af_info *afi,
					  const struct nlattr *nla)
{
	struct nft_table *table;

	list_for_each_entry(table, &afi->tables, list) {
		if (!nla_strcmp(nla, table->name))
			return table;
	}
	return NULL;
}