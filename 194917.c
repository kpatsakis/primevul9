static int nft_add_set_elem(struct nft_ctx *ctx, struct nft_set *set,
			    const struct nlattr *attr)
{
	struct nlattr *nla[NFTA_SET_ELEM_MAX + 1];
	struct nft_data_desc d1, d2;
	struct nft_set_elem elem;
	struct nft_set_binding *binding;
	enum nft_registers dreg;
	struct nft_trans *trans;
	int err;

	if (set->size && set->nelems == set->size)
		return -ENFILE;

	err = nla_parse_nested(nla, NFTA_SET_ELEM_MAX, attr,
			       nft_set_elem_policy);
	if (err < 0)
		return err;

	if (nla[NFTA_SET_ELEM_KEY] == NULL)
		return -EINVAL;

	elem.flags = 0;
	if (nla[NFTA_SET_ELEM_FLAGS] != NULL) {
		elem.flags = ntohl(nla_get_be32(nla[NFTA_SET_ELEM_FLAGS]));
		if (elem.flags & ~NFT_SET_ELEM_INTERVAL_END)
			return -EINVAL;
	}

	if (set->flags & NFT_SET_MAP) {
		if (nla[NFTA_SET_ELEM_DATA] == NULL &&
		    !(elem.flags & NFT_SET_ELEM_INTERVAL_END))
			return -EINVAL;
		if (nla[NFTA_SET_ELEM_DATA] != NULL &&
		    elem.flags & NFT_SET_ELEM_INTERVAL_END)
			return -EINVAL;
	} else {
		if (nla[NFTA_SET_ELEM_DATA] != NULL)
			return -EINVAL;
	}

	err = nft_data_init(ctx, &elem.key, &d1, nla[NFTA_SET_ELEM_KEY]);
	if (err < 0)
		goto err1;
	err = -EINVAL;
	if (d1.type != NFT_DATA_VALUE || d1.len != set->klen)
		goto err2;

	err = -EEXIST;
	if (set->ops->get(set, &elem) == 0)
		goto err2;

	if (nla[NFTA_SET_ELEM_DATA] != NULL) {
		err = nft_data_init(ctx, &elem.data, &d2, nla[NFTA_SET_ELEM_DATA]);
		if (err < 0)
			goto err2;

		err = -EINVAL;
		if (set->dtype != NFT_DATA_VERDICT && d2.len != set->dlen)
			goto err3;

		dreg = nft_type_to_reg(set->dtype);
		list_for_each_entry(binding, &set->bindings, list) {
			struct nft_ctx bind_ctx = {
				.afi	= ctx->afi,
				.table	= ctx->table,
				.chain	= (struct nft_chain *)binding->chain,
			};

			err = nft_validate_data_load(&bind_ctx, dreg,
						     &elem.data, d2.type);
			if (err < 0)
				goto err3;
		}
	}

	trans = nft_trans_elem_alloc(ctx, NFT_MSG_NEWSETELEM, set);
	if (trans == NULL)
		goto err3;

	err = set->ops->insert(set, &elem);
	if (err < 0)
		goto err4;

	nft_trans_elem(trans) = elem;
	list_add_tail(&trans->list, &ctx->net->nft.commit_list);
	return 0;

err4:
	kfree(trans);
err3:
	if (nla[NFTA_SET_ELEM_DATA] != NULL)
		nft_data_uninit(&elem.data, d2.type);
err2:
	nft_data_uninit(&elem.key, d1.type);
err1:
	return err;
}