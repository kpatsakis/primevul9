static int nft_add_set_elem(struct nft_ctx *ctx, struct nft_set *set,
			    const struct nlattr *attr, u32 nlmsg_flags)
{
	struct nft_expr *expr_array[NFT_SET_EXPR_MAX] = {};
	struct nlattr *nla[NFTA_SET_ELEM_MAX + 1];
	u8 genmask = nft_genmask_next(ctx->net);
	u32 flags = 0, size = 0, num_exprs = 0;
	struct nft_set_ext_tmpl tmpl;
	struct nft_set_ext *ext, *ext2;
	struct nft_set_elem elem;
	struct nft_set_binding *binding;
	struct nft_object *obj = NULL;
	struct nft_userdata *udata;
	struct nft_data_desc desc;
	enum nft_registers dreg;
	struct nft_trans *trans;
	u64 timeout;
	u64 expiration;
	int err, i;
	u8 ulen;

	err = nla_parse_nested_deprecated(nla, NFTA_SET_ELEM_MAX, attr,
					  nft_set_elem_policy, NULL);
	if (err < 0)
		return err;

	nft_set_ext_prepare(&tmpl);

	err = nft_setelem_parse_flags(set, nla[NFTA_SET_ELEM_FLAGS], &flags);
	if (err < 0)
		return err;

	if (!nla[NFTA_SET_ELEM_KEY] && !(flags & NFT_SET_ELEM_CATCHALL))
		return -EINVAL;

	if (flags != 0) {
		err = nft_set_ext_add(&tmpl, NFT_SET_EXT_FLAGS);
		if (err < 0)
			return err;
	}

	if (set->flags & NFT_SET_MAP) {
		if (nla[NFTA_SET_ELEM_DATA] == NULL &&
		    !(flags & NFT_SET_ELEM_INTERVAL_END))
			return -EINVAL;
	} else {
		if (nla[NFTA_SET_ELEM_DATA] != NULL)
			return -EINVAL;
	}

	if (set->flags & NFT_SET_OBJECT) {
		if (!nla[NFTA_SET_ELEM_OBJREF] &&
		    !(flags & NFT_SET_ELEM_INTERVAL_END))
			return -EINVAL;
	} else {
		if (nla[NFTA_SET_ELEM_OBJREF])
			return -EINVAL;
	}

	if (!nft_setelem_valid_key_end(set, nla, flags))
		return -EINVAL;

	if ((flags & NFT_SET_ELEM_INTERVAL_END) &&
	     (nla[NFTA_SET_ELEM_DATA] ||
	      nla[NFTA_SET_ELEM_OBJREF] ||
	      nla[NFTA_SET_ELEM_TIMEOUT] ||
	      nla[NFTA_SET_ELEM_EXPIRATION] ||
	      nla[NFTA_SET_ELEM_USERDATA] ||
	      nla[NFTA_SET_ELEM_EXPR] ||
	      nla[NFTA_SET_ELEM_KEY_END] ||
	      nla[NFTA_SET_ELEM_EXPRESSIONS]))
		return -EINVAL;

	timeout = 0;
	if (nla[NFTA_SET_ELEM_TIMEOUT] != NULL) {
		if (!(set->flags & NFT_SET_TIMEOUT))
			return -EINVAL;
		err = nf_msecs_to_jiffies64(nla[NFTA_SET_ELEM_TIMEOUT],
					    &timeout);
		if (err)
			return err;
	} else if (set->flags & NFT_SET_TIMEOUT) {
		timeout = set->timeout;
	}

	expiration = 0;
	if (nla[NFTA_SET_ELEM_EXPIRATION] != NULL) {
		if (!(set->flags & NFT_SET_TIMEOUT))
			return -EINVAL;
		err = nf_msecs_to_jiffies64(nla[NFTA_SET_ELEM_EXPIRATION],
					    &expiration);
		if (err)
			return err;
	}

	if (nla[NFTA_SET_ELEM_EXPR]) {
		struct nft_expr *expr;

		if (set->num_exprs && set->num_exprs != 1)
			return -EOPNOTSUPP;

		expr = nft_set_elem_expr_alloc(ctx, set,
					       nla[NFTA_SET_ELEM_EXPR]);
		if (IS_ERR(expr))
			return PTR_ERR(expr);

		expr_array[0] = expr;
		num_exprs = 1;

		if (set->num_exprs && set->exprs[0]->ops != expr->ops) {
			err = -EOPNOTSUPP;
			goto err_set_elem_expr;
		}
	} else if (nla[NFTA_SET_ELEM_EXPRESSIONS]) {
		struct nft_expr *expr;
		struct nlattr *tmp;
		int left;

		i = 0;
		nla_for_each_nested(tmp, nla[NFTA_SET_ELEM_EXPRESSIONS], left) {
			if (i == NFT_SET_EXPR_MAX ||
			    (set->num_exprs && set->num_exprs == i)) {
				err = -E2BIG;
				goto err_set_elem_expr;
			}
			if (nla_type(tmp) != NFTA_LIST_ELEM) {
				err = -EINVAL;
				goto err_set_elem_expr;
			}
			expr = nft_set_elem_expr_alloc(ctx, set, tmp);
			if (IS_ERR(expr)) {
				err = PTR_ERR(expr);
				goto err_set_elem_expr;
			}
			expr_array[i] = expr;
			num_exprs++;

			if (set->num_exprs && expr->ops != set->exprs[i]->ops) {
				err = -EOPNOTSUPP;
				goto err_set_elem_expr;
			}
			i++;
		}
		if (set->num_exprs && set->num_exprs != i) {
			err = -EOPNOTSUPP;
			goto err_set_elem_expr;
		}
	} else if (set->num_exprs > 0) {
		err = nft_set_elem_expr_clone(ctx, set, expr_array);
		if (err < 0)
			goto err_set_elem_expr_clone;

		num_exprs = set->num_exprs;
	}

	if (nla[NFTA_SET_ELEM_KEY]) {
		err = nft_setelem_parse_key(ctx, set, &elem.key.val,
					    nla[NFTA_SET_ELEM_KEY]);
		if (err < 0)
			goto err_set_elem_expr;

		err = nft_set_ext_add_length(&tmpl, NFT_SET_EXT_KEY, set->klen);
		if (err < 0)
			goto err_parse_key;
	}

	if (nla[NFTA_SET_ELEM_KEY_END]) {
		err = nft_setelem_parse_key(ctx, set, &elem.key_end.val,
					    nla[NFTA_SET_ELEM_KEY_END]);
		if (err < 0)
			goto err_parse_key;

		err = nft_set_ext_add_length(&tmpl, NFT_SET_EXT_KEY_END, set->klen);
		if (err < 0)
			goto err_parse_key_end;
	}

	if (timeout > 0) {
		err = nft_set_ext_add(&tmpl, NFT_SET_EXT_EXPIRATION);
		if (err < 0)
			goto err_parse_key_end;

		if (timeout != set->timeout) {
			err = nft_set_ext_add(&tmpl, NFT_SET_EXT_TIMEOUT);
			if (err < 0)
				goto err_parse_key_end;
		}
	}

	if (num_exprs) {
		for (i = 0; i < num_exprs; i++)
			size += expr_array[i]->ops->size;

		err = nft_set_ext_add_length(&tmpl, NFT_SET_EXT_EXPRESSIONS,
					     sizeof(struct nft_set_elem_expr) + size);
		if (err < 0)
			goto err_parse_key_end;
	}

	if (nla[NFTA_SET_ELEM_OBJREF] != NULL) {
		obj = nft_obj_lookup(ctx->net, ctx->table,
				     nla[NFTA_SET_ELEM_OBJREF],
				     set->objtype, genmask);
		if (IS_ERR(obj)) {
			err = PTR_ERR(obj);
			goto err_parse_key_end;
		}
		err = nft_set_ext_add(&tmpl, NFT_SET_EXT_OBJREF);
		if (err < 0)
			goto err_parse_key_end;
	}

	if (nla[NFTA_SET_ELEM_DATA] != NULL) {
		err = nft_setelem_parse_data(ctx, set, &desc, &elem.data.val,
					     nla[NFTA_SET_ELEM_DATA]);
		if (err < 0)
			goto err_parse_key_end;

		dreg = nft_type_to_reg(set->dtype);
		list_for_each_entry(binding, &set->bindings, list) {
			struct nft_ctx bind_ctx = {
				.net	= ctx->net,
				.family	= ctx->family,
				.table	= ctx->table,
				.chain	= (struct nft_chain *)binding->chain,
			};

			if (!(binding->flags & NFT_SET_MAP))
				continue;

			err = nft_validate_register_store(&bind_ctx, dreg,
							  &elem.data.val,
							  desc.type, desc.len);
			if (err < 0)
				goto err_parse_data;

			if (desc.type == NFT_DATA_VERDICT &&
			    (elem.data.val.verdict.code == NFT_GOTO ||
			     elem.data.val.verdict.code == NFT_JUMP))
				nft_validate_state_update(ctx->net,
							  NFT_VALIDATE_NEED);
		}

		err = nft_set_ext_add_length(&tmpl, NFT_SET_EXT_DATA, desc.len);
		if (err < 0)
			goto err_parse_data;
	}

	/* The full maximum length of userdata can exceed the maximum
	 * offset value (U8_MAX) for following extensions, therefor it
	 * must be the last extension added.
	 */
	ulen = 0;
	if (nla[NFTA_SET_ELEM_USERDATA] != NULL) {
		ulen = nla_len(nla[NFTA_SET_ELEM_USERDATA]);
		if (ulen > 0) {
			err = nft_set_ext_add_length(&tmpl, NFT_SET_EXT_USERDATA,
						     ulen);
			if (err < 0)
				goto err_parse_data;
		}
	}

	elem.priv = nft_set_elem_init(set, &tmpl, elem.key.val.data,
				      elem.key_end.val.data, elem.data.val.data,
				      timeout, expiration, GFP_KERNEL_ACCOUNT);
	if (IS_ERR(elem.priv)) {
		err = PTR_ERR(elem.priv);
		goto err_parse_data;
	}

	ext = nft_set_elem_ext(set, elem.priv);
	if (flags)
		*nft_set_ext_flags(ext) = flags;

	if (ulen > 0) {
		if (nft_set_ext_check(&tmpl, NFT_SET_EXT_USERDATA, ulen) < 0) {
			err = -EINVAL;
			goto err_elem_userdata;
		}
		udata = nft_set_ext_userdata(ext);
		udata->len = ulen - 1;
		nla_memcpy(&udata->data, nla[NFTA_SET_ELEM_USERDATA], ulen);
	}
	if (obj) {
		*nft_set_ext_obj(ext) = obj;
		obj->use++;
	}
	err = nft_set_elem_expr_setup(ctx, &tmpl, ext, expr_array, num_exprs);
	if (err < 0)
		goto err_elem_free;

	trans = nft_trans_elem_alloc(ctx, NFT_MSG_NEWSETELEM, set);
	if (trans == NULL) {
		err = -ENOMEM;
		goto err_elem_free;
	}

	ext->genmask = nft_genmask_cur(ctx->net) | NFT_SET_ELEM_BUSY_MASK;

	err = nft_setelem_insert(ctx->net, set, &elem, &ext2, flags);
	if (err) {
		if (err == -EEXIST) {
			if (nft_set_ext_exists(ext, NFT_SET_EXT_DATA) ^
			    nft_set_ext_exists(ext2, NFT_SET_EXT_DATA) ||
			    nft_set_ext_exists(ext, NFT_SET_EXT_OBJREF) ^
			    nft_set_ext_exists(ext2, NFT_SET_EXT_OBJREF))
				goto err_element_clash;
			if ((nft_set_ext_exists(ext, NFT_SET_EXT_DATA) &&
			     nft_set_ext_exists(ext2, NFT_SET_EXT_DATA) &&
			     memcmp(nft_set_ext_data(ext),
				    nft_set_ext_data(ext2), set->dlen) != 0) ||
			    (nft_set_ext_exists(ext, NFT_SET_EXT_OBJREF) &&
			     nft_set_ext_exists(ext2, NFT_SET_EXT_OBJREF) &&
			     *nft_set_ext_obj(ext) != *nft_set_ext_obj(ext2)))
				goto err_element_clash;
			else if (!(nlmsg_flags & NLM_F_EXCL))
				err = 0;
		} else if (err == -ENOTEMPTY) {
			/* ENOTEMPTY reports overlapping between this element
			 * and an existing one.
			 */
			err = -EEXIST;
		}
		goto err_element_clash;
	}

	if (!(flags & NFT_SET_ELEM_CATCHALL) && set->size &&
	    !atomic_add_unless(&set->nelems, 1, set->size + set->ndeact)) {
		err = -ENFILE;
		goto err_set_full;
	}

	nft_trans_elem(trans) = elem;
	nft_trans_commit_list_add_tail(ctx->net, trans);
	return 0;

err_set_full:
	nft_setelem_remove(ctx->net, set, &elem);
err_element_clash:
	kfree(trans);
err_elem_free:
	if (obj)
		obj->use--;
err_elem_userdata:
	nf_tables_set_elem_destroy(ctx, set, elem.priv);
err_parse_data:
	if (nla[NFTA_SET_ELEM_DATA] != NULL)
		nft_data_release(&elem.data.val, desc.type);
err_parse_key_end:
	nft_data_release(&elem.key_end.val, NFT_DATA_VALUE);
err_parse_key:
	nft_data_release(&elem.key.val, NFT_DATA_VALUE);
err_set_elem_expr:
	for (i = 0; i < num_exprs && expr_array[i]; i++)
		nft_expr_destroy(ctx, expr_array[i]);
err_set_elem_expr_clone:
	return err;
}