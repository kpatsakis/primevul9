static void nfsd4_init_copy_res(struct nfsd4_copy *copy, bool sync)
{
	copy->cp_res.wr_stable_how = NFS_UNSTABLE;
	copy->cp_synchronous = sync;
	gen_boot_verifier(&copy->cp_res.wr_verifier, copy->cp_clp->net);
}