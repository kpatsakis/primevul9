static inline int rndis_get_nr(void)
{
	return ida_simple_get(&rndis_ida, 0, 0, GFP_KERNEL);
}