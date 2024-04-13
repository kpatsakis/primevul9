static void nfs4_free_createdata(struct nfs4_createdata *data)
{
	kfree(data);
}