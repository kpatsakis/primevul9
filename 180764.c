void __init page_alloc_init(void)
{
	int ret;

	ret = cpuhp_setup_state_nocalls(CPUHP_PAGE_ALLOC_DEAD,
					"mm/page_alloc:dead", NULL,
					page_alloc_cpu_dead);
	WARN_ON(ret < 0);
}