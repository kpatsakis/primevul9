static void nf_tables_table_destroy(struct nft_ctx *ctx)
{
	BUG_ON(ctx->table->use > 0);

	kfree(ctx->table);
	module_put(ctx->afi->owner);
}