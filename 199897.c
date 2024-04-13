static inline void dec_snd_pages(int order)
{
	snd_allocated_pages -= 1 << order;
}