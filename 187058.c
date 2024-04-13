static int nfs4_recover_expired_lease(struct nfs_server *server)
{
	return nfs4_client_recover_expired_lease(server->nfs_client);
}