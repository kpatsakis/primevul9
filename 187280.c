static void nfs4_layoutreturn_release(void *calldata)
{
	struct nfs4_layoutreturn *lrp = calldata;

	dprintk("--> %s\n", __func__);
	put_layout_hdr(lrp->args.layout);
	kfree(calldata);
	dprintk("<-- %s\n", __func__);
}