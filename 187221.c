static u32 nfs4_xdr_minorversion(const struct nfs4_sequence_args *args)
{
#if defined(CONFIG_NFS_V4_1)
	if (args->sa_session)
		return args->sa_session->clp->cl_mvops->minor_version;
#endif /* CONFIG_NFS_V4_1 */
	return 0;
}