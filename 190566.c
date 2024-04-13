static inline bool __remain_node_summaries(int reason)
{
	return (reason & (CP_UMOUNT | CP_FASTBOOT));
}