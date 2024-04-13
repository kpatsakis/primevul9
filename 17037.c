static int nft_set_elem_expr_setup(struct nft_ctx *ctx,
				   const struct nft_set_ext_tmpl *tmpl,
				   const struct nft_set_ext *ext,
				   struct nft_expr *expr_array[],
				   u32 num_exprs)
{
	struct nft_set_elem_expr *elem_expr = nft_set_ext_expr(ext);
	u32 len = sizeof(struct nft_set_elem_expr);
	struct nft_expr *expr;
	int i, err;

	if (num_exprs == 0)
		return 0;

	for (i = 0; i < num_exprs; i++)
		len += expr_array[i]->ops->size;

	if (nft_set_ext_check(tmpl, NFT_SET_EXT_EXPRESSIONS, len) < 0)
		return -EINVAL;

	for (i = 0; i < num_exprs; i++) {
		expr = nft_setelem_expr_at(elem_expr, elem_expr->size);
		err = nft_expr_clone(expr, expr_array[i]);
		if (err < 0)
			goto err_elem_expr_setup;

		elem_expr->size += expr_array[i]->ops->size;
		nft_expr_destroy(ctx, expr_array[i]);
		expr_array[i] = NULL;
	}

	return 0;

err_elem_expr_setup:
	for (; i < num_exprs; i++) {
		nft_expr_destroy(ctx, expr_array[i]);
		expr_array[i] = NULL;
	}

	return -ENOMEM;
}