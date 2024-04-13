void nf_tables_unbind_set(const struct nft_ctx *ctx, struct nft_set *set,
			  struct nft_set_binding *binding)
{
	list_del_rcu(&binding->list);

	if (list_empty(&set->bindings) && set->flags & NFT_SET_ANONYMOUS &&
	    !(set->flags & NFT_SET_INACTIVE))
		nf_tables_set_destroy(ctx, set);
}