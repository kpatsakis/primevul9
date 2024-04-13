static void nf_tables_unregister_hooks(const struct nft_table *table,
				       const struct nft_chain *chain,
				       unsigned int hook_nops)
{
	if (!(table->flags & NFT_TABLE_F_DORMANT) &&
	    chain->flags & NFT_BASE_CHAIN)
		nf_unregister_hooks(nft_base_chain(chain)->ops, hook_nops);
}