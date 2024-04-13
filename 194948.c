static int nf_tables_dump_sets(struct sk_buff *skb, struct netlink_callback *cb)
{
	const struct nft_set *set;
	unsigned int idx, s_idx = cb->args[0];
	struct nft_af_info *afi;
	struct nft_table *table, *cur_table = (struct nft_table *)cb->args[2];
	struct net *net = sock_net(skb->sk);
	int cur_family = cb->args[3];
	struct nft_ctx *ctx = cb->data, ctx_set;

	if (cb->args[1])
		return skb->len;

	rcu_read_lock();
	cb->seq = net->nft.base_seq;

	list_for_each_entry_rcu(afi, &net->nft.af_info, list) {
		if (ctx->afi && ctx->afi != afi)
			continue;

		if (cur_family) {
			if (afi->family != cur_family)
				continue;

			cur_family = 0;
		}
		list_for_each_entry_rcu(table, &afi->tables, list) {
			if (ctx->table && ctx->table != table)
				continue;

			if (cur_table) {
				if (cur_table != table)
					continue;

				cur_table = NULL;
			}
			idx = 0;
			list_for_each_entry_rcu(set, &table->sets, list) {
				if (idx < s_idx)
					goto cont;

				ctx_set = *ctx;
				ctx_set.table = table;
				ctx_set.afi = afi;
				if (nf_tables_fill_set(skb, &ctx_set, set,
						       NFT_MSG_NEWSET,
						       NLM_F_MULTI) < 0) {
					cb->args[0] = idx;
					cb->args[2] = (unsigned long) table;
					cb->args[3] = afi->family;
					goto done;
				}
				nl_dump_check_consistent(cb, nlmsg_hdr(skb));
cont:
				idx++;
			}
			if (s_idx)
				s_idx = 0;
		}
	}
	cb->args[1] = 1;
done:
	rcu_read_unlock();
	return skb->len;
}