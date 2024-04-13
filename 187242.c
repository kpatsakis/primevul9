static int nfs4_do_setattr(struct inode *inode, struct rpc_cred *cred,
			   struct nfs_fattr *fattr, struct iattr *sattr,
			   struct nfs4_state *state)
{
	struct nfs_server *server = NFS_SERVER(inode);
	struct nfs4_exception exception = {
		.state = state,
		.inode = inode,
	};
	int err;
	do {
		err = _nfs4_do_setattr(inode, cred, fattr, sattr, state);
		switch (err) {
		case -NFS4ERR_OPENMODE:
			if (state && !(state->state & FMODE_WRITE)) {
				err = -EBADF;
				if (sattr->ia_valid & ATTR_OPEN)
					err = -EACCES;
				goto out;
			}
		}
		err = nfs4_handle_exception(server, err, &exception);
	} while (exception.retry);
out:
	return err;
}