static void clear_sp_write_flooding_count(u64 *spte)
{
	__clear_sp_write_flooding_count(sptep_to_sp(spte));
}