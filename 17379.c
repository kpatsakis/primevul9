static int follow_dotdot_rcu(struct nameidata *nd)
{
	set_root_rcu(nd);

	while (1) {
		if (nd->path.dentry == nd->root.dentry &&
		    nd->path.mnt == nd->root.mnt) {
			break;
		}
		if (nd->path.dentry != nd->path.mnt->mnt_root) {
			struct dentry *old = nd->path.dentry;
			struct dentry *parent = old->d_parent;
			unsigned seq;

			seq = read_seqcount_begin(&parent->d_seq);
			if (read_seqcount_retry(&old->d_seq, nd->seq))
				goto failed;
			nd->path.dentry = parent;
			nd->seq = seq;
			break;
		}
		if (!follow_up_rcu(&nd->path))
			break;
		nd->seq = read_seqcount_begin(&nd->path.dentry->d_seq);
	}
	follow_mount_rcu(nd);
	nd->inode = nd->path.dentry->d_inode;
	return 0;

failed:
	nd->flags &= ~LOOKUP_RCU;
	if (!(nd->flags & LOOKUP_ROOT))
		nd->root.mnt = NULL;
	unlock_rcu_walk();
	return -ECHILD;
}