static int _nfs4_proc_mknod(struct inode *dir, struct dentry *dentry,
		struct iattr *sattr, dev_t rdev)
{
	struct nfs4_createdata *data;
	int mode = sattr->ia_mode;
	int status = -ENOMEM;

	BUG_ON(!(sattr->ia_valid & ATTR_MODE));
	BUG_ON(!S_ISFIFO(mode) && !S_ISBLK(mode) && !S_ISCHR(mode) && !S_ISSOCK(mode));

	data = nfs4_alloc_createdata(dir, &dentry->d_name, sattr, NF4SOCK);
	if (data == NULL)
		goto out;

	if (S_ISFIFO(mode))
		data->arg.ftype = NF4FIFO;
	else if (S_ISBLK(mode)) {
		data->arg.ftype = NF4BLK;
		data->arg.u.device.specdata1 = MAJOR(rdev);
		data->arg.u.device.specdata2 = MINOR(rdev);
	}
	else if (S_ISCHR(mode)) {
		data->arg.ftype = NF4CHR;
		data->arg.u.device.specdata1 = MAJOR(rdev);
		data->arg.u.device.specdata2 = MINOR(rdev);
	}
	
	status = nfs4_do_create(dir, dentry, data);

	nfs4_free_createdata(data);
out:
	return status;
}