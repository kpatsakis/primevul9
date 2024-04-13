static void renew_lease(const struct nfs_server *server, unsigned long timestamp)
{
	do_renew_lease(server->nfs_client, timestamp);
}