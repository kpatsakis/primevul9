int nf_tables_bind_set(const struct nft_ctx *ctx, struct nft_set *set,
		       struct nft_set_binding *binding)
{
	struct nft_set_binding *i;
	struct nft_set_iter iter;

	if (!list_empty(&set->bindings) && set->flags & NFT_SET_ANONYMOUS)
		return -EBUSY;

	if (set->flags & NFT_SET_MAP) {
		/* If the set is already bound to the same chain all
		 * jumps are already validated for that chain.
		 */
		list_for_each_entry(i, &set->bindings, list) {
			if (i->chain == binding->chain)
				goto bind;
		}

		iter.skip 	= 0;
		iter.count	= 0;
		iter.err	= 0;
		iter.fn		= nf_tables_bind_check_setelem;

		set->ops->walk(ctx, set, &iter);
		if (iter.err < 0) {
			/* Destroy anonymous sets if binding fails */
			if (set->flags & NFT_SET_ANONYMOUS)
				nf_tables_set_destroy(ctx, set);

			return iter.err;
		}
	}
bind:
	binding->chain = ctx->chain;
	list_add_tail_rcu(&binding->list, &set->bindings);
	return 0;
}