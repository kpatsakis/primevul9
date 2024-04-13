static int nf_tables_commit(struct net *net, struct sk_buff *skb)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_trans *trans, *next;
	struct nft_trans_elem *te;
	struct nft_chain *chain;
	struct nft_table *table;
	unsigned int base_seq;
	LIST_HEAD(adl);
	int err;

	if (list_empty(&nft_net->commit_list)) {
		mutex_unlock(&nft_net->commit_mutex);
		return 0;
	}

	/* 0. Validate ruleset, otherwise roll back for error reporting. */
	if (nf_tables_validate(net) < 0)
		return -EAGAIN;

	err = nft_flow_rule_offload_commit(net);
	if (err < 0)
		return err;

	/* 1.  Allocate space for next generation rules_gen_X[] */
	list_for_each_entry_safe(trans, next, &nft_net->commit_list, list) {
		int ret;

		ret = nf_tables_commit_audit_alloc(&adl, trans->ctx.table);
		if (ret) {
			nf_tables_commit_chain_prepare_cancel(net);
			nf_tables_commit_audit_free(&adl);
			return ret;
		}
		if (trans->msg_type == NFT_MSG_NEWRULE ||
		    trans->msg_type == NFT_MSG_DELRULE) {
			chain = trans->ctx.chain;

			ret = nf_tables_commit_chain_prepare(net, chain);
			if (ret < 0) {
				nf_tables_commit_chain_prepare_cancel(net);
				nf_tables_commit_audit_free(&adl);
				return ret;
			}
		}
	}

	/* step 2.  Make rules_gen_X visible to packet path */
	list_for_each_entry(table, &nft_net->tables, list) {
		list_for_each_entry(chain, &table->chains, list)
			nf_tables_commit_chain(net, chain);
	}

	/*
	 * Bump generation counter, invalidate any dump in progress.
	 * Cannot fail after this point.
	 */
	base_seq = READ_ONCE(nft_net->base_seq);
	while (++base_seq == 0)
		;

	WRITE_ONCE(nft_net->base_seq, base_seq);

	/* step 3. Start new generation, rules_gen_X now in use. */
	net->nft.gencursor = nft_gencursor_next(net);

	list_for_each_entry_safe(trans, next, &nft_net->commit_list, list) {
		nf_tables_commit_audit_collect(&adl, trans->ctx.table,
					       trans->msg_type);
		switch (trans->msg_type) {
		case NFT_MSG_NEWTABLE:
			if (nft_trans_table_update(trans)) {
				if (!(trans->ctx.table->flags & __NFT_TABLE_F_UPDATE)) {
					nft_trans_destroy(trans);
					break;
				}
				if (trans->ctx.table->flags & NFT_TABLE_F_DORMANT)
					nf_tables_table_disable(net, trans->ctx.table);

				trans->ctx.table->flags &= ~__NFT_TABLE_F_UPDATE;
			} else {
				nft_clear(net, trans->ctx.table);
			}
			nf_tables_table_notify(&trans->ctx, NFT_MSG_NEWTABLE);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELTABLE:
			list_del_rcu(&trans->ctx.table->list);
			nf_tables_table_notify(&trans->ctx, NFT_MSG_DELTABLE);
			break;
		case NFT_MSG_NEWCHAIN:
			if (nft_trans_chain_update(trans)) {
				nft_chain_commit_update(trans);
				nf_tables_chain_notify(&trans->ctx, NFT_MSG_NEWCHAIN);
				/* trans destroyed after rcu grace period */
			} else {
				nft_chain_commit_drop_policy(trans);
				nft_clear(net, trans->ctx.chain);
				nf_tables_chain_notify(&trans->ctx, NFT_MSG_NEWCHAIN);
				nft_trans_destroy(trans);
			}
			break;
		case NFT_MSG_DELCHAIN:
			nft_chain_del(trans->ctx.chain);
			nf_tables_chain_notify(&trans->ctx, NFT_MSG_DELCHAIN);
			nf_tables_unregister_hook(trans->ctx.net,
						  trans->ctx.table,
						  trans->ctx.chain);
			break;
		case NFT_MSG_NEWRULE:
			nft_clear(trans->ctx.net, nft_trans_rule(trans));
			nf_tables_rule_notify(&trans->ctx,
					      nft_trans_rule(trans),
					      NFT_MSG_NEWRULE);
			if (trans->ctx.chain->flags & NFT_CHAIN_HW_OFFLOAD)
				nft_flow_rule_destroy(nft_trans_flow_rule(trans));

			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELRULE:
			list_del_rcu(&nft_trans_rule(trans)->list);
			nf_tables_rule_notify(&trans->ctx,
					      nft_trans_rule(trans),
					      NFT_MSG_DELRULE);
			nft_rule_expr_deactivate(&trans->ctx,
						 nft_trans_rule(trans),
						 NFT_TRANS_COMMIT);
			break;
		case NFT_MSG_NEWSET:
			nft_clear(net, nft_trans_set(trans));
			/* This avoids hitting -EBUSY when deleting the table
			 * from the transaction.
			 */
			if (nft_set_is_anonymous(nft_trans_set(trans)) &&
			    !list_empty(&nft_trans_set(trans)->bindings))
				trans->ctx.table->use--;

			nf_tables_set_notify(&trans->ctx, nft_trans_set(trans),
					     NFT_MSG_NEWSET, GFP_KERNEL);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELSET:
			list_del_rcu(&nft_trans_set(trans)->list);
			nf_tables_set_notify(&trans->ctx, nft_trans_set(trans),
					     NFT_MSG_DELSET, GFP_KERNEL);
			break;
		case NFT_MSG_NEWSETELEM:
			te = (struct nft_trans_elem *)trans->data;

			nft_setelem_activate(net, te->set, &te->elem);
			nf_tables_setelem_notify(&trans->ctx, te->set,
						 &te->elem,
						 NFT_MSG_NEWSETELEM);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELSETELEM:
			te = (struct nft_trans_elem *)trans->data;

			nf_tables_setelem_notify(&trans->ctx, te->set,
						 &te->elem,
						 NFT_MSG_DELSETELEM);
			nft_setelem_remove(net, te->set, &te->elem);
			if (!nft_setelem_is_catchall(te->set, &te->elem)) {
				atomic_dec(&te->set->nelems);
				te->set->ndeact--;
			}
			break;
		case NFT_MSG_NEWOBJ:
			if (nft_trans_obj_update(trans)) {
				nft_obj_commit_update(trans);
				nf_tables_obj_notify(&trans->ctx,
						     nft_trans_obj(trans),
						     NFT_MSG_NEWOBJ);
			} else {
				nft_clear(net, nft_trans_obj(trans));
				nf_tables_obj_notify(&trans->ctx,
						     nft_trans_obj(trans),
						     NFT_MSG_NEWOBJ);
				nft_trans_destroy(trans);
			}
			break;
		case NFT_MSG_DELOBJ:
			nft_obj_del(nft_trans_obj(trans));
			nf_tables_obj_notify(&trans->ctx, nft_trans_obj(trans),
					     NFT_MSG_DELOBJ);
			break;
		case NFT_MSG_NEWFLOWTABLE:
			if (nft_trans_flowtable_update(trans)) {
				nft_trans_flowtable(trans)->data.flags =
					nft_trans_flowtable_flags(trans);
				nf_tables_flowtable_notify(&trans->ctx,
							   nft_trans_flowtable(trans),
							   &nft_trans_flowtable_hooks(trans),
							   NFT_MSG_NEWFLOWTABLE);
				list_splice(&nft_trans_flowtable_hooks(trans),
					    &nft_trans_flowtable(trans)->hook_list);
			} else {
				nft_clear(net, nft_trans_flowtable(trans));
				nf_tables_flowtable_notify(&trans->ctx,
							   nft_trans_flowtable(trans),
							   &nft_trans_flowtable(trans)->hook_list,
							   NFT_MSG_NEWFLOWTABLE);
			}
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_DELFLOWTABLE:
			if (nft_trans_flowtable_update(trans)) {
				nf_tables_flowtable_notify(&trans->ctx,
							   nft_trans_flowtable(trans),
							   &nft_trans_flowtable_hooks(trans),
							   NFT_MSG_DELFLOWTABLE);
				nft_unregister_flowtable_net_hooks(net,
								   &nft_trans_flowtable_hooks(trans));
			} else {
				list_del_rcu(&nft_trans_flowtable(trans)->list);
				nf_tables_flowtable_notify(&trans->ctx,
							   nft_trans_flowtable(trans),
							   &nft_trans_flowtable(trans)->hook_list,
							   NFT_MSG_DELFLOWTABLE);
				nft_unregister_flowtable_net_hooks(net,
						&nft_trans_flowtable(trans)->hook_list);
			}
			break;
		}
	}

	nft_commit_notify(net, NETLINK_CB(skb).portid);
	nf_tables_gen_notify(net, skb, NFT_MSG_NEWGEN);
	nf_tables_commit_audit_log(&adl, nft_net->base_seq);
	nf_tables_commit_release(net);

	return 0;
}