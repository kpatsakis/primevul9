static inline void __init pgdat_init_report_one_done(void)
{
	if (atomic_dec_and_test(&pgdat_init_n_undone))
		complete(&pgdat_init_all_done_comp);
}