static void drain_pages(unsigned int cpu)
{
	struct zone *zone;

	for_each_populated_zone(zone) {
		drain_pages_zone(cpu, zone);
	}
}