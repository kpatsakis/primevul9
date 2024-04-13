void nfsd4_shutdown_copy(struct nfs4_client *clp)
{
	struct nfsd4_copy *copy;

	while ((copy = nfsd4_get_copy(clp)) != NULL)
		nfsd4_stop_copy(copy);
}