static int sg_allow_access(struct file *filp, unsigned char *cmd)
{
	struct sg_fd *sfp = filp->private_data;

	if (sfp->parentdp->device->type == TYPE_SCANNER)
		return 0;

	return blk_verify_command(cmd, filp->f_mode & FMODE_WRITE);
}