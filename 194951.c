static int nft_flush(struct nft_ctx *ctx, int family)
{
	struct nft_af_info *afi;
	struct nft_table *table, *nt;
	const struct nlattr * const *nla = ctx->nla;
	int err = 0;

	list_for_each_entry(afi, &ctx->net->nft.af_info, list) {
		if (family != AF_UNSPEC && afi->family != family)
			continue;

		ctx->afi = afi;
		list_for_each_entry_safe(table, nt, &afi->tables, list) {
			if (nla[NFTA_TABLE_NAME] &&
			    nla_strcmp(nla[NFTA_TABLE_NAME], table->name) != 0)
				continue;

			ctx->table = table;

			err = nft_flush_table(ctx);
			if (err < 0)
				goto out;
		}
	}
out:
	return err;
}