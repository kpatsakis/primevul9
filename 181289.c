static void cleanup_async_copy(struct nfsd4_copy *copy)
{
	nfs4_free_cp_state(copy);
	fput(copy->file_dst);
	fput(copy->file_src);
	spin_lock(&copy->cp_clp->async_lock);
	list_del(&copy->copies);
	spin_unlock(&copy->cp_clp->async_lock);
	nfs4_put_copy(copy);
}