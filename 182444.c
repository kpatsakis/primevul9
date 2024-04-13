smb2_copy_fs_info_to_kstatfs(struct smb2_fs_full_size_info *pfs_inf,
			     struct kstatfs *kst)
{
	kst->f_bsize = le32_to_cpu(pfs_inf->BytesPerSector) *
			  le32_to_cpu(pfs_inf->SectorsPerAllocationUnit);
	kst->f_blocks = le64_to_cpu(pfs_inf->TotalAllocationUnits);
	kst->f_bfree  = kst->f_bavail =
			le64_to_cpu(pfs_inf->CallerAvailableAllocationUnits);
	return;
}