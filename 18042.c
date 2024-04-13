static bool file_has_brlocks(files_struct *fsp)
{
	struct byte_range_lock *br_lck;

	br_lck = brl_get_locks_readonly(fsp);
	if (!br_lck)
		return false;

	return (brl_num_locks(br_lck) > 0);
}