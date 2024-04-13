int nft_set_elem_expr_clone(const struct nft_ctx *ctx, struct nft_set *set,
			    struct nft_expr *expr_array[])
{
	struct nft_expr *expr;
	int err, i, k;

	for (i = 0; i < set->num_exprs; i++) {
		expr = kzalloc(set->exprs[i]->ops->size, GFP_KERNEL_ACCOUNT);
		if (!expr)
			goto err_expr;

		err = nft_expr_clone(expr, set->exprs[i]);
		if (err < 0) {
			kfree(expr);
			goto err_expr;
		}
		expr_array[i] = expr;
	}

	return 0;

err_expr:
	for (k = i - 1; k >= 0; k--)
		nft_expr_destroy(ctx, expr_array[k]);

	return -ENOMEM;
}