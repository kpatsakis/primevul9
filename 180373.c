static inline int __init dccp_mib_init(void)
{
	dccp_statistics = alloc_percpu(struct dccp_mib);
	if (!dccp_statistics)
		return -ENOMEM;
	return 0;
}