static void nf_tables_table_disable(const struct nft_af_info *afi,
				   struct nft_table *table)
{
	struct nft_chain *chain;

	list_for_each_entry(chain, &table->chains, list) {
		if (chain->flags & NFT_BASE_CHAIN)
			nf_unregister_hooks(nft_base_chain(chain)->ops,
					    afi->nops);
	}
}