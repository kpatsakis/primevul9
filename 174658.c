vcs_size(struct inode *inode)
{
	int size;
	struct vc_data *vc;

	WARN_CONSOLE_UNLOCKED();

	vc = vcs_vc(inode, NULL);
	if (!vc)
		return -ENXIO;

	size = vc->vc_rows * vc->vc_cols;

	if (use_attributes(inode)) {
		if (use_unicode(inode))
			return -EOPNOTSUPP;
		size = 2*size + HEADER_SIZE;
	} else if (use_unicode(inode))
		size *= 4;
	return size;
}