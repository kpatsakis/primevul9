static inline bool should_suppress_show_mem(void)
{
	bool ret = false;

#if NODES_SHIFT > 8
	ret = in_interrupt();
#endif
	return ret;
}