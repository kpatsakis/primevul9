static int nft_del_setelem(struct nft_ctx *ctx, struct nft_set *set,
			   const struct nlattr *attr)
{
	struct nlattr *nla[NFTA_SET_ELEM_MAX + 1];
	struct nft_data_desc desc;
	struct nft_set_elem elem;
	struct nft_trans *trans;
	int err;

	err = nla_parse_nested(nla, NFTA_SET_ELEM_MAX, attr,
			       nft_set_elem_policy);
	if (err < 0)
		goto err1;

	err = -EINVAL;
	if (nla[NFTA_SET_ELEM_KEY] == NULL)
		goto err1;

	err = nft_data_init(ctx, &elem.key, &desc, nla[NFTA_SET_ELEM_KEY]);
	if (err < 0)
		goto err1;

	err = -EINVAL;
	if (desc.type != NFT_DATA_VALUE || desc.len != set->klen)
		goto err2;

	err = set->ops->get(set, &elem);
	if (err < 0)
		goto err2;

	trans = nft_trans_elem_alloc(ctx, NFT_MSG_DELSETELEM, set);
	if (trans == NULL) {
		err = -ENOMEM;
		goto err2;
	}

	nft_trans_elem(trans) = elem;
	list_add_tail(&trans->list, &ctx->net->nft.commit_list);
	return 0;
err2:
	nft_data_uninit(&elem.key, desc.type);
err1:
	return err;
}