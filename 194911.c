static int nf_tables_table_enable(const struct nft_af_info *afi,
				  struct nft_table *table)
{
	struct nft_chain *chain;
	int err, i = 0;

	list_for_each_entry(chain, &table->chains, list) {
		if (!(chain->flags & NFT_BASE_CHAIN))
			continue;

		err = nf_register_hooks(nft_base_chain(chain)->ops, afi->nops);
		if (err < 0)
			goto err;

		i++;
	}
	return 0;
err:
	list_for_each_entry(chain, &table->chains, list) {
		if (!(chain->flags & NFT_BASE_CHAIN))
			continue;

		if (i-- <= 0)
			break;

		nf_unregister_hooks(nft_base_chain(chain)->ops, afi->nops);
	}
	return err;
}