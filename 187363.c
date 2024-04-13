static size_t max_response_pages(struct nfs_server *server)
{
	u32 max_resp_sz = server->nfs_client->cl_session->fc_attrs.max_resp_sz;
	return nfs_page_array_len(0, max_resp_sz);
}