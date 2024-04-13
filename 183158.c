static struct file *aio_private_file(struct kioctx *ctx, loff_t nr_pages)
{
	struct qstr this = QSTR_INIT("[aio]", 5);
	struct file *file;
	struct path path;
	struct inode *inode = alloc_anon_inode(aio_mnt->mnt_sb);
	if (IS_ERR(inode))
		return ERR_CAST(inode);

	inode->i_mapping->a_ops = &aio_ctx_aops;
	inode->i_mapping->private_data = ctx;
	inode->i_mapping->backing_dev_info = &aio_fs_backing_dev_info;
	inode->i_size = PAGE_SIZE * nr_pages;

	path.dentry = d_alloc_pseudo(aio_mnt->mnt_sb, &this);
	if (!path.dentry) {
		iput(inode);
		return ERR_PTR(-ENOMEM);
	}
	path.mnt = mntget(aio_mnt);

	d_instantiate(path.dentry, inode);
	file = alloc_file(&path, FMODE_READ | FMODE_WRITE, &aio_ring_fops);
	if (IS_ERR(file)) {
		path_put(&path);
		return file;
	}

	file->f_flags = O_RDWR;
	file->private_data = ctx;
	return file;
}