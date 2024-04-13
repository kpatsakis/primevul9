static int nf_tables_dump_setelem(const struct nft_ctx *ctx,
				  const struct nft_set *set,
				  const struct nft_set_iter *iter,
				  const struct nft_set_elem *elem)
{
	struct nft_set_dump_args *args;

	args = container_of(iter, struct nft_set_dump_args, iter);
	return nf_tables_fill_setelem(args->skb, set, elem);
}