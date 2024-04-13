static int nf_tables_abort(struct sk_buff *skb)
{
	struct net *net = sock_net(skb->sk);
	struct nft_trans *trans, *next;
	struct nft_set *set;

	list_for_each_entry_safe(trans, next, &net->nft.commit_list, list) {
		switch (trans->msg_type) {
		case NFT_MSG_NEWTABLE:
			if (nft_trans_table_update(trans)) {
				if (nft_trans_table_enable(trans)) {
					nf_tables_table_disable(trans->ctx.afi,
								trans->ctx.table);
					trans->ctx.table->flags |= NFT_TABLE_F_DORMANT;
				}
				nft_trans_destroy(trans);
			} else {
				list_del_rcu(&trans->ctx.table->list);
			}
			break;
		case NFT_MSG_DELTABLE:
			list_add_tail_rcu(&trans->ctx.table->list,
					  &trans->ctx.afi->tables);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWCHAIN:
			if (nft_trans_chain_update(trans)) {
				free_percpu(nft_trans_chain_stats(trans));

				nft_trans_destroy(trans);
			} else {
				trans->ctx.table->use--;
				list_del_rcu(&trans->ctx.chain->list);
				nf_tables_unregister_hooks(trans->ctx.table,
							   trans->ctx.chain,
							   trans->ctx.afi->nops);
			}
			break;
		case NFT_MSG_DELCHAIN:
			trans->ctx.table->use++;
			list_add_tail_rcu(&trans->ctx.chain->list,
					  &trans->ctx.table->chains);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWRULE:
			trans->ctx.chain->use--;
			list_del_rcu(&nft_trans_rule(trans)->list);
			break;
		case NFT_MSG_DELRULE:
			trans->ctx.chain->use++;
			nft_rule_clear(trans->ctx.net, nft_trans_rule(trans));
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWSET:
			trans->ctx.table->use--;
			list_del_rcu(&nft_trans_set(trans)->list);
			break;
		case NFT_MSG_DELSET:
			trans->ctx.table->use++;
			list_add_tail_rcu(&nft_trans_set(trans)->list,
					  &trans->ctx.table->sets);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWSETELEM:
			nft_trans_elem_set(trans)->nelems--;
			set = nft_trans_elem_set(trans);
			set->ops->get(set, &nft_trans_elem(trans));
			set->ops->remove(set, &nft_trans_elem(trans));
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELSETELEM:
			nft_trans_elem_set(trans)->nelems++;
			nft_trans_destroy(trans);
			break;
		}
	}

	synchronize_rcu();

	list_for_each_entry_safe_reverse(trans, next,
					 &net->nft.commit_list, list) {
		list_del(&trans->list);
		nf_tables_abort_release(trans);
	}

	return 0;
}