static void sun6i_a31_get_pll6_factors(struct factors_request *req)
{
	u8 div;

	/* Normalize value to a parent_rate multiple (24M) */
	div = req->rate / req->parent_rate;
	req->rate = req->parent_rate * div;

	req->k = div / 32;
	if (req->k > 3)
		req->k = 3;

	req->n = DIV_ROUND_UP(div, (req->k + 1)) - 1;
}