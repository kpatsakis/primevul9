copy_posix_fs_info_to_kstatfs(FILE_SYSTEM_POSIX_INFO *response_data,
			struct kstatfs *kst)
{
	kst->f_bsize = le32_to_cpu(response_data->BlockSize);
	kst->f_blocks = le64_to_cpu(response_data->TotalBlocks);
	kst->f_bfree =  le64_to_cpu(response_data->BlocksAvail);
	if (response_data->UserBlocksAvail == cpu_to_le64(-1))
		kst->f_bavail = kst->f_bfree;
	else
		kst->f_bavail = le64_to_cpu(response_data->UserBlocksAvail);
	if (response_data->TotalFileNodes != cpu_to_le64(-1))
		kst->f_files = le64_to_cpu(response_data->TotalFileNodes);
	if (response_data->FreeFileNodes != cpu_to_le64(-1))
		kst->f_ffree = le64_to_cpu(response_data->FreeFileNodes);

	return;
}