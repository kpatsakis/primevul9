static int nf_tables_addchain(struct nft_ctx *ctx, u8 family, u8 genmask,
			      u8 policy, u32 flags,
			      struct netlink_ext_ack *extack)
{
	const struct nlattr * const *nla = ctx->nla;
	struct nft_stats __percpu *stats = NULL;
	struct nft_table *table = ctx->table;
	struct nft_base_chain *basechain;
	struct net *net = ctx->net;
	char name[NFT_NAME_MAXLEN];
	struct nft_rule_blob *blob;
	struct nft_trans *trans;
	struct nft_chain *chain;
	unsigned int data_size;
	int err;

	if (table->use == UINT_MAX)
		return -EOVERFLOW;

	if (nla[NFTA_CHAIN_HOOK]) {
		struct nft_chain_hook hook;

		if (flags & NFT_CHAIN_BINDING)
			return -EOPNOTSUPP;

		err = nft_chain_parse_hook(net, nla, &hook, family, extack,
					   true);
		if (err < 0)
			return err;

		basechain = kzalloc(sizeof(*basechain), GFP_KERNEL_ACCOUNT);
		if (basechain == NULL) {
			nft_chain_release_hook(&hook);
			return -ENOMEM;
		}
		chain = &basechain->chain;

		if (nla[NFTA_CHAIN_COUNTERS]) {
			stats = nft_stats_alloc(nla[NFTA_CHAIN_COUNTERS]);
			if (IS_ERR(stats)) {
				nft_chain_release_hook(&hook);
				kfree(basechain);
				return PTR_ERR(stats);
			}
			rcu_assign_pointer(basechain->stats, stats);
		}

		err = nft_basechain_init(basechain, family, &hook, flags);
		if (err < 0) {
			nft_chain_release_hook(&hook);
			kfree(basechain);
			return err;
		}
	} else {
		if (flags & NFT_CHAIN_BASE)
			return -EINVAL;
		if (flags & NFT_CHAIN_HW_OFFLOAD)
			return -EOPNOTSUPP;

		chain = kzalloc(sizeof(*chain), GFP_KERNEL_ACCOUNT);
		if (chain == NULL)
			return -ENOMEM;

		chain->flags = flags;
	}
	ctx->chain = chain;

	INIT_LIST_HEAD(&chain->rules);
	chain->handle = nf_tables_alloc_handle(table);
	chain->table = table;

	if (nla[NFTA_CHAIN_NAME]) {
		chain->name = nla_strdup(nla[NFTA_CHAIN_NAME], GFP_KERNEL_ACCOUNT);
	} else {
		if (!(flags & NFT_CHAIN_BINDING)) {
			err = -EINVAL;
			goto err_destroy_chain;
		}

		snprintf(name, sizeof(name), "__chain%llu", ++chain_id);
		chain->name = kstrdup(name, GFP_KERNEL_ACCOUNT);
	}

	if (!chain->name) {
		err = -ENOMEM;
		goto err_destroy_chain;
	}

	if (nla[NFTA_CHAIN_USERDATA]) {
		chain->udata = nla_memdup(nla[NFTA_CHAIN_USERDATA], GFP_KERNEL_ACCOUNT);
		if (chain->udata == NULL) {
			err = -ENOMEM;
			goto err_destroy_chain;
		}
		chain->udlen = nla_len(nla[NFTA_CHAIN_USERDATA]);
	}

	data_size = offsetof(struct nft_rule_dp, data);	/* last rule */
	blob = nf_tables_chain_alloc_rules(data_size);
	if (!blob) {
		err = -ENOMEM;
		goto err_destroy_chain;
	}

	RCU_INIT_POINTER(chain->blob_gen_0, blob);
	RCU_INIT_POINTER(chain->blob_gen_1, blob);

	err = nf_tables_register_hook(net, table, chain);
	if (err < 0)
		goto err_destroy_chain;

	trans = nft_trans_chain_add(ctx, NFT_MSG_NEWCHAIN);
	if (IS_ERR(trans)) {
		err = PTR_ERR(trans);
		goto err_unregister_hook;
	}

	nft_trans_chain_policy(trans) = NFT_CHAIN_POLICY_UNSET;
	if (nft_is_base_chain(chain))
		nft_trans_chain_policy(trans) = policy;

	err = nft_chain_add(table, chain);
	if (err < 0) {
		nft_trans_destroy(trans);
		goto err_unregister_hook;
	}

	if (stats)
		static_branch_inc(&nft_counters_enabled);

	table->use++;

	return 0;
err_unregister_hook:
	nf_tables_unregister_hook(net, table, chain);
err_destroy_chain:
	nf_tables_chain_destroy(ctx);

	return err;
}