static void sun6i_display_factors(struct factors_request *req)
{
	u8 m;

	if (req->rate > req->parent_rate)
		req->rate = req->parent_rate;

	m = DIV_ROUND_UP(req->parent_rate, req->rate);

	req->rate = req->parent_rate / m;
	req->m = m - 1;
}