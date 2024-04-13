static inline unsigned long long cur_cp_version(struct f2fs_checkpoint *cp)
{
	return le64_to_cpu(cp->checkpoint_ver);
}