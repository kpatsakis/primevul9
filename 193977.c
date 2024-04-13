static void handle_bad_sector(struct bio *bio, sector_t maxsector)
{
	char b[BDEVNAME_SIZE];

	printk(KERN_INFO "attempt to access beyond end of device\n");
	printk(KERN_INFO "%s: rw=%d, want=%Lu, limit=%Lu\n",
			bio_devname(bio, b), bio->bi_opf,
			(unsigned long long)bio_end_sector(bio),
			(long long)maxsector);
}