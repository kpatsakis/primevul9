static void gen_boot_verifier(nfs4_verifier *verifier, struct net *net)
{
	__be32 verf[2];
	struct nfsd_net *nn = net_generic(net, nfsd_net_id);

	/*
	 * This is opaque to client, so no need to byte-swap. Use
	 * __force to keep sparse happy
	 */
	verf[0] = (__force __be32)nn->nfssvc_boot.tv_sec;
	verf[1] = (__force __be32)nn->nfssvc_boot.tv_usec;
	memcpy(verifier->data, verf, sizeof(verifier->data));
}