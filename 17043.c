static int nf_tables_dump_rules(struct sk_buff *skb,
				struct netlink_callback *cb)
{
	const struct nfgenmsg *nfmsg = nlmsg_data(cb->nlh);
	const struct nft_rule_dump_ctx *ctx = cb->data;
	struct nft_table *table;
	const struct nft_chain *chain;
	unsigned int idx = 0;
	struct net *net = sock_net(skb->sk);
	int family = nfmsg->nfgen_family;
	struct nftables_pernet *nft_net;

	rcu_read_lock();
	nft_net = nft_pernet(net);
	cb->seq = READ_ONCE(nft_net->base_seq);

	list_for_each_entry_rcu(table, &nft_net->tables, list) {
		if (family != NFPROTO_UNSPEC && family != table->family)
			continue;

		if (ctx && ctx->table && strcmp(ctx->table, table->name) != 0)
			continue;

		if (ctx && ctx->table && ctx->chain) {
			struct rhlist_head *list, *tmp;

			list = rhltable_lookup(&table->chains_ht, ctx->chain,
					       nft_chain_ht_params);
			if (!list)
				goto done;

			rhl_for_each_entry_rcu(chain, tmp, list, rhlhead) {
				if (!nft_is_active(net, chain))
					continue;
				__nf_tables_dump_rules(skb, &idx,
						       cb, table, chain);
				break;
			}
			goto done;
		}

		list_for_each_entry_rcu(chain, &table->chains, list) {
			if (__nf_tables_dump_rules(skb, &idx, cb, table, chain))
				goto done;
		}

		if (ctx && ctx->table)
			break;
	}
done:
	rcu_read_unlock();

	cb->args[0] = idx;
	return skb->len;
}