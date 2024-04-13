static void put_aio_ring_file(struct kioctx *ctx)
{
	struct file *aio_ring_file = ctx->aio_ring_file;
	if (aio_ring_file) {
		truncate_setsize(aio_ring_file->f_inode, 0);

		/* Prevent further access to the kioctx from migratepages */
		spin_lock(&aio_ring_file->f_inode->i_mapping->private_lock);
		aio_ring_file->f_inode->i_mapping->private_data = NULL;
		ctx->aio_ring_file = NULL;
		spin_unlock(&aio_ring_file->f_inode->i_mapping->private_lock);

		fput(aio_ring_file);
	}
}