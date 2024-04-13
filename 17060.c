static int nft_del_setelem(struct nft_ctx *ctx, struct nft_set *set,
			   const struct nlattr *attr)
{
	struct nlattr *nla[NFTA_SET_ELEM_MAX + 1];
	struct nft_set_ext_tmpl tmpl;
	struct nft_set_elem elem;
	struct nft_set_ext *ext;
	struct nft_trans *trans;
	u32 flags = 0;
	int err;

	err = nla_parse_nested_deprecated(nla, NFTA_SET_ELEM_MAX, attr,
					  nft_set_elem_policy, NULL);
	if (err < 0)
		return err;

	err = nft_setelem_parse_flags(set, nla[NFTA_SET_ELEM_FLAGS], &flags);
	if (err < 0)
		return err;

	if (!nla[NFTA_SET_ELEM_KEY] && !(flags & NFT_SET_ELEM_CATCHALL))
		return -EINVAL;

	if (!nft_setelem_valid_key_end(set, nla, flags))
		return -EINVAL;

	nft_set_ext_prepare(&tmpl);

	if (flags != 0) {
		err = nft_set_ext_add(&tmpl, NFT_SET_EXT_FLAGS);
		if (err < 0)
			return err;
	}

	if (nla[NFTA_SET_ELEM_KEY]) {
		err = nft_setelem_parse_key(ctx, set, &elem.key.val,
					    nla[NFTA_SET_ELEM_KEY]);
		if (err < 0)
			return err;

		err = nft_set_ext_add_length(&tmpl, NFT_SET_EXT_KEY, set->klen);
		if (err < 0)
			goto fail_elem;
	}

	if (nla[NFTA_SET_ELEM_KEY_END]) {
		err = nft_setelem_parse_key(ctx, set, &elem.key_end.val,
					    nla[NFTA_SET_ELEM_KEY_END]);
		if (err < 0)
			goto fail_elem;

		err = nft_set_ext_add_length(&tmpl, NFT_SET_EXT_KEY_END, set->klen);
		if (err < 0)
			goto fail_elem_key_end;
	}

	err = -ENOMEM;
	elem.priv = nft_set_elem_init(set, &tmpl, elem.key.val.data,
				      elem.key_end.val.data, NULL, 0, 0,
				      GFP_KERNEL_ACCOUNT);
	if (IS_ERR(elem.priv)) {
		err = PTR_ERR(elem.priv);
		goto fail_elem_key_end;
	}

	ext = nft_set_elem_ext(set, elem.priv);
	if (flags)
		*nft_set_ext_flags(ext) = flags;

	trans = nft_trans_elem_alloc(ctx, NFT_MSG_DELSETELEM, set);
	if (trans == NULL)
		goto fail_trans;

	err = nft_setelem_deactivate(ctx->net, set, &elem, flags);
	if (err < 0)
		goto fail_ops;

	nft_setelem_data_deactivate(ctx->net, set, &elem);

	nft_trans_elem(trans) = elem;
	nft_trans_commit_list_add_tail(ctx->net, trans);
	return 0;

fail_ops:
	kfree(trans);
fail_trans:
	kfree(elem.priv);
fail_elem_key_end:
	nft_data_release(&elem.key_end.val, NFT_DATA_VALUE);
fail_elem:
	nft_data_release(&elem.key.val, NFT_DATA_VALUE);
	return err;
}