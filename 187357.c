static void nfs4_opendata_free(struct kref *kref)
{
	struct nfs4_opendata *p = container_of(kref,
			struct nfs4_opendata, kref);
	struct super_block *sb = p->dentry->d_sb;

	nfs_free_seqid(p->o_arg.seqid);
	if (p->state != NULL)
		nfs4_put_open_state(p->state);
	nfs4_put_state_owner(p->owner);
	dput(p->dir);
	dput(p->dentry);
	nfs_sb_deactive(sb);
	nfs_fattr_free_names(&p->f_attr);
	kfree(p);
}