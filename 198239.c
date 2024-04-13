static void sun5i_a13_get_ahb_factors(struct factors_request *req)
{
	u32 div;

	/* divide only */
	if (req->parent_rate < req->rate)
		req->rate = req->parent_rate;

	/*
	 * user manual says valid speed is 8k ~ 276M, but tests show it
	 * can work at speeds up to 300M, just after reparenting to pll6
	 */
	if (req->rate < 8000)
		req->rate = 8000;
	if (req->rate > 300000000)
		req->rate = 300000000;

	div = order_base_2(DIV_ROUND_UP(req->parent_rate, req->rate));

	/* p = 0 ~ 3 */
	if (div > 3)
		div = 3;

	req->rate = req->parent_rate >> div;

	req->p = div;
}