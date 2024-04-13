static int nf_tables_commit(struct sk_buff *skb)
{
	struct net *net = sock_net(skb->sk);
	struct nft_trans *trans, *next;
	struct nft_trans_elem *te;

	/* Bump generation counter, invalidate any dump in progress */
	while (++net->nft.base_seq == 0);

	/* A new generation has just started */
	net->nft.gencursor = gencursor_next(net);

	/* Make sure all packets have left the previous generation before
	 * purging old rules.
	 */
	synchronize_rcu();

	list_for_each_entry_safe(trans, next, &net->nft.commit_list, list) {
		switch (trans->msg_type) {
		case NFT_MSG_NEWTABLE:
			if (nft_trans_table_update(trans)) {
				if (!nft_trans_table_enable(trans)) {
					nf_tables_table_disable(trans->ctx.afi,
								trans->ctx.table);
					trans->ctx.table->flags |= NFT_TABLE_F_DORMANT;
				}
			} else {
				trans->ctx.table->flags &= ~NFT_TABLE_INACTIVE;
			}
			nf_tables_table_notify(&trans->ctx, NFT_MSG_NEWTABLE);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELTABLE:
			nf_tables_table_notify(&trans->ctx, NFT_MSG_DELTABLE);
			break;
		case NFT_MSG_NEWCHAIN:
			if (nft_trans_chain_update(trans))
				nft_chain_commit_update(trans);
			else
				trans->ctx.chain->flags &= ~NFT_CHAIN_INACTIVE;

			nf_tables_chain_notify(&trans->ctx, NFT_MSG_NEWCHAIN);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELCHAIN:
			nf_tables_chain_notify(&trans->ctx, NFT_MSG_DELCHAIN);
			nf_tables_unregister_hooks(trans->ctx.table,
						   trans->ctx.chain,
						   trans->ctx.afi->nops);
			break;
		case NFT_MSG_NEWRULE:
			nft_rule_clear(trans->ctx.net, nft_trans_rule(trans));
			nf_tables_rule_notify(&trans->ctx,
					      nft_trans_rule(trans),
					      NFT_MSG_NEWRULE);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELRULE:
			list_del_rcu(&nft_trans_rule(trans)->list);
			nf_tables_rule_notify(&trans->ctx,
					      nft_trans_rule(trans),
					      NFT_MSG_DELRULE);
			break;
		case NFT_MSG_NEWSET:
			nft_trans_set(trans)->flags &= ~NFT_SET_INACTIVE;
			/* This avoids hitting -EBUSY when deleting the table
			 * from the transaction.
			 */
			if (nft_trans_set(trans)->flags & NFT_SET_ANONYMOUS &&
			    !list_empty(&nft_trans_set(trans)->bindings))
				trans->ctx.table->use--;

			nf_tables_set_notify(&trans->ctx, nft_trans_set(trans),
					     NFT_MSG_NEWSET, GFP_KERNEL);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELSET:
			nf_tables_set_notify(&trans->ctx, nft_trans_set(trans),
					     NFT_MSG_DELSET, GFP_KERNEL);
			break;
		case NFT_MSG_NEWSETELEM:
			nf_tables_setelem_notify(&trans->ctx,
						 nft_trans_elem_set(trans),
						 &nft_trans_elem(trans),
						 NFT_MSG_NEWSETELEM, 0);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELSETELEM:
			te = (struct nft_trans_elem *)trans->data;
			nf_tables_setelem_notify(&trans->ctx, te->set,
						 &te->elem,
						 NFT_MSG_DELSETELEM, 0);
			te->set->ops->get(te->set, &te->elem);
			te->set->ops->remove(te->set, &te->elem);
			nft_data_uninit(&te->elem.key, NFT_DATA_VALUE);
			if (te->elem.flags & NFT_SET_MAP) {
				nft_data_uninit(&te->elem.data,
						te->set->dtype);
			}
			nft_trans_destroy(trans);
			break;
		}
	}

	synchronize_rcu();

	list_for_each_entry_safe(trans, next, &net->nft.commit_list, list) {
		list_del(&trans->list);
		nf_tables_commit_release(trans);
	}

	nf_tables_gen_notify(net, skb, NFT_MSG_NEWGEN);

	return 0;
}