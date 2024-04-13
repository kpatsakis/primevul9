static inline void dccp_mib_exit(void)
{
	free_percpu(dccp_statistics);
}