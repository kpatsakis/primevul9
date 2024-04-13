static int selinux_mmap_file(struct file *file, unsigned long reqprot,
			     unsigned long prot, unsigned long flags)
{
	struct common_audit_data ad;
	int rc;

	if (file) {
		ad.type = LSM_AUDIT_DATA_FILE;
		ad.u.file = file;
		rc = inode_has_perm(current_cred(), file_inode(file),
				    FILE__MAP, &ad);
		if (rc)
			return rc;
	}

	if (selinux_state.checkreqprot)
		prot = reqprot;

	return file_map_prot_check(file, prot,
				   (flags & MAP_TYPE) == MAP_SHARED);
}