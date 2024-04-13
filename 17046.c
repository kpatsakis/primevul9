static int nf_tables_dump_obj(struct sk_buff *skb, struct netlink_callback *cb)
{
	const struct nfgenmsg *nfmsg = nlmsg_data(cb->nlh);
	const struct nft_table *table;
	unsigned int idx = 0, s_idx = cb->args[0];
	struct nft_obj_filter *filter = cb->data;
	struct net *net = sock_net(skb->sk);
	int family = nfmsg->nfgen_family;
	struct nftables_pernet *nft_net;
	struct nft_object *obj;
	bool reset = false;

	if (NFNL_MSG_TYPE(cb->nlh->nlmsg_type) == NFT_MSG_GETOBJ_RESET)
		reset = true;

	rcu_read_lock();
	nft_net = nft_pernet(net);
	cb->seq = READ_ONCE(nft_net->base_seq);

	list_for_each_entry_rcu(table, &nft_net->tables, list) {
		if (family != NFPROTO_UNSPEC && family != table->family)
			continue;

		list_for_each_entry_rcu(obj, &table->objects, list) {
			if (!nft_is_active(net, obj))
				goto cont;
			if (idx < s_idx)
				goto cont;
			if (idx > s_idx)
				memset(&cb->args[1], 0,
				       sizeof(cb->args) - sizeof(cb->args[0]));
			if (filter && filter->table &&
			    strcmp(filter->table, table->name))
				goto cont;
			if (filter &&
			    filter->type != NFT_OBJECT_UNSPEC &&
			    obj->ops->type->type != filter->type)
				goto cont;
			if (reset) {
				char *buf = kasprintf(GFP_ATOMIC,
						      "%s:%u",
						      table->name,
						      nft_net->base_seq);

				audit_log_nfcfg(buf,
						family,
						obj->handle,
						AUDIT_NFT_OP_OBJ_RESET,
						GFP_ATOMIC);
				kfree(buf);
			}

			if (nf_tables_fill_obj_info(skb, net, NETLINK_CB(cb->skb).portid,
						    cb->nlh->nlmsg_seq,
						    NFT_MSG_NEWOBJ,
						    NLM_F_MULTI | NLM_F_APPEND,
						    table->family, table,
						    obj, reset) < 0)
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