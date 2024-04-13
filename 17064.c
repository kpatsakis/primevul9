static int nf_tables_dump_chains(struct sk_buff *skb,
				 struct netlink_callback *cb)
{
	const struct nfgenmsg *nfmsg = nlmsg_data(cb->nlh);
	unsigned int idx = 0, s_idx = cb->args[0];
	struct net *net = sock_net(skb->sk);
	int family = nfmsg->nfgen_family;
	struct nftables_pernet *nft_net;
	const struct nft_table *table;
	const struct nft_chain *chain;

	rcu_read_lock();
	nft_net = nft_pernet(net);
	cb->seq = READ_ONCE(nft_net->base_seq);

	list_for_each_entry_rcu(table, &nft_net->tables, list) {
		if (family != NFPROTO_UNSPEC && family != table->family)
			continue;

		list_for_each_entry_rcu(chain, &table->chains, list) {
			if (idx < s_idx)
				goto cont;
			if (idx > s_idx)
				memset(&cb->args[1], 0,
				       sizeof(cb->args) - sizeof(cb->args[0]));
			if (!nft_is_active(net, chain))
				continue;
			if (nf_tables_fill_chain_info(skb, net,
						      NETLINK_CB(cb->skb).portid,
						      cb->nlh->nlmsg_seq,
						      NFT_MSG_NEWCHAIN,
						      NLM_F_MULTI,
						      table->family, table,
						      chain) < 0)
				goto done;

			nl_dump_check_consistent(cb, nlmsg_hdr(skb));
cont:
			idx++;
		}
	}
done:
	rcu_read_unlock();
	cb->args[0] = idx;
	return skb->len;
}