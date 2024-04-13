static void setup_pageset(struct per_cpu_pageset *p, unsigned long batch)
{
	pageset_init(p);
	pageset_set_batch(p, batch);
}