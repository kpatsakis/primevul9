static struct nfs4_opendata *nfs4_opendata_alloc(struct dentry *dentry,
		struct nfs4_state_owner *sp, fmode_t fmode, int flags,
		const struct iattr *attrs,
		gfp_t gfp_mask)
{
	struct dentry *parent = dget_parent(dentry);
	struct inode *dir = parent->d_inode;
	struct nfs_server *server = NFS_SERVER(dir);
	struct nfs4_opendata *p;

	p = kzalloc(sizeof(*p), gfp_mask);
	if (p == NULL)
		goto err;
	p->o_arg.seqid = nfs_alloc_seqid(&sp->so_seqid, gfp_mask);
	if (p->o_arg.seqid == NULL)
		goto err_free;
	nfs_sb_active(dentry->d_sb);
	p->dentry = dget(dentry);
	p->dir = parent;
	p->owner = sp;
	atomic_inc(&sp->so_count);
	p->o_arg.fh = NFS_FH(dir);
	p->o_arg.open_flags = flags;
	p->o_arg.fmode = fmode & (FMODE_READ|FMODE_WRITE);
	p->o_arg.clientid = server->nfs_client->cl_clientid;
	p->o_arg.id.create_time = ktime_to_ns(sp->so_seqid.create_time);
	p->o_arg.id.uniquifier = sp->so_seqid.owner_id;
	p->o_arg.name = &dentry->d_name;
	p->o_arg.server = server;
	p->o_arg.bitmask = server->attr_bitmask;
	p->o_arg.open_bitmap = &nfs4_fattr_bitmap[0];
	p->o_arg.claim = NFS4_OPEN_CLAIM_NULL;
	if (attrs != NULL && attrs->ia_valid != 0) {
		__be32 verf[2];

		p->o_arg.u.attrs = &p->attrs;
		memcpy(&p->attrs, attrs, sizeof(p->attrs));

		verf[0] = jiffies;
		verf[1] = current->pid;
		memcpy(p->o_arg.u.verifier.data, verf,
				sizeof(p->o_arg.u.verifier.data));
	}
	p->c_arg.fh = &p->o_res.fh;
	p->c_arg.stateid = &p->o_res.stateid;
	p->c_arg.seqid = p->o_arg.seqid;
	nfs4_init_opendata_res(p);
	kref_init(&p->kref);
	return p;
err_free:
	kfree(p);
err:
	dput(parent);
	return NULL;
}