static void nf_tables_chain_destroy(struct nft_chain *chain)
{
	BUG_ON(chain->use > 0);

	if (chain->flags & NFT_BASE_CHAIN) {
		module_put(nft_base_chain(chain)->type->owner);
		free_percpu(nft_base_chain(chain)->stats);
		kfree(nft_base_chain(chain));
	} else {
		kfree(chain);
	}
}