nf_tables_chain_lookup_byhandle(const struct nft_table *table, u64 handle)
{
	struct nft_chain *chain;

	list_for_each_entry(chain, &table->chains, list) {
		if (chain->handle == handle)
			return chain;
	}

	return ERR_PTR(-ENOENT);
}