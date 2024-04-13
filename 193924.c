static inline int bio_check_eod(struct bio *bio, sector_t maxsector)
{
	unsigned int nr_sectors = bio_sectors(bio);

	if (nr_sectors && maxsector &&
	    (nr_sectors > maxsector ||
	     bio->bi_iter.bi_sector > maxsector - nr_sectors)) {
		handle_bad_sector(bio, maxsector);
		return -EIO;
	}
	return 0;
}