static inline int nfs4_server_supports_acls(struct nfs_server *server)
{
	return (server->caps & NFS_CAP_ACLS)
		&& (server->acl_bitmask & ACL4_SUPPORT_ALLOW_ACL)
		&& (server->acl_bitmask & ACL4_SUPPORT_DENY_ACL);
}