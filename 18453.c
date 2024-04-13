void add_disk_randomness(struct gendisk *disk)
{
	if (!disk || !disk->random)
		return;
	/* first major is 1, so we get >= 0x200 here */
	add_timer_randomness(disk->random, 0x100 + disk_devt(disk));
	trace_add_disk_randomness(disk_devt(disk), ENTROPY_BITS(&input_pool));
}