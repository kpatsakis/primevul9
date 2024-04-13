static inline void rndis_put_nr(int nr)
{
	ida_simple_remove(&rndis_ida, nr);
}