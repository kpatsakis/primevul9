static void destroy_request_data(struct request_data *req)
{
	if (req->timeout > 0)
		g_source_remove(req->timeout);

	g_free(req->resp);
	g_free(req->request);
	g_free(req->name);
	g_free(req);
}