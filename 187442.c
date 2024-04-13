void __init mdesc_get_page_sizes(cpumask_t *mask, unsigned long *pgsz_mask)
{
	*pgsz_mask = 0;
	mdesc_iterate_over_cpus(check_one_pgsz, pgsz_mask, mask);
}