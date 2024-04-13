static void pageset_set_high(struct per_cpu_pageset *p,
				unsigned long high)
{
	unsigned long batch = max(1UL, high / 4);
	if ((high / 4) > (PAGE_SHIFT * 8))
		batch = PAGE_SHIFT * 8;

	pageset_update(&p->pcp, high, batch);
}