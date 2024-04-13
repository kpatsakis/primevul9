static void numa_crng_init(void)
{
	schedule_work(&numa_crng_init_work);
}