static void sun4i_get_pll5_factors(struct factors_request *req)
{
	u8 div;

	/* Normalize value to a parent_rate multiple (24M) */
	div = req->rate / req->parent_rate;
	req->rate = req->parent_rate * div;

	if (div < 31)
		req->k = 0;
	else if (div / 2 < 31)
		req->k = 1;
	else if (div / 3 < 31)
		req->k = 2;
	else
		req->k = 3;

	req->n = DIV_ROUND_UP(div, (req->k + 1));
}