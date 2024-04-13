static int nf_tables_dump_rules(struct sk_buff *skb,
				struct netlink_callback *cb)
{
	const struct nfgenmsg *nfmsg = nlmsg_data(cb->nlh);
	const struct nft_af_info *afi;
	const struct nft_table *table;
	const struct nft_chain *chain;
	const struct nft_rule *rule;
	unsigned int idx = 0, s_idx = cb->args[0];
	struct net *net = sock_net(skb->sk);
	int family = nfmsg->nfgen_family;

	rcu_read_lock();
	cb->seq = net->nft.base_seq;

	list_for_each_entry_rcu(afi, &net->nft.af_info, list) {
		if (family != NFPROTO_UNSPEC && family != afi->family)
			continue;

		list_for_each_entry_rcu(table, &afi->tables, list) {
			list_for_each_entry_rcu(chain, &table->chains, list) {
				list_for_each_entry_rcu(rule, &chain->rules, list) {
					if (!nft_rule_is_active(net, rule))
						goto cont;
					if (idx < s_idx)
						goto cont;
					if (idx > s_idx)
						memset(&cb->args[1], 0,
						       sizeof(cb->args) - sizeof(cb->args[0]));
					if (nf_tables_fill_rule_info(skb, net, NETLINK_CB(cb->skb).portid,
								      cb->nlh->nlmsg_seq,
								      NFT_MSG_NEWRULE,
								      NLM_F_MULTI | NLM_F_APPEND,
								      afi->family, table, chain, rule) < 0)
						goto done;

					nl_dump_check_consistent(cb, nlmsg_hdr(skb));
cont:
					idx++;
				}
			}
		}
	}
done:
	rcu_read_unlock();

	cb->args[0] = idx;
	return skb->len;
}