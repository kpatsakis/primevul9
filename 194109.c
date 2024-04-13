static noinline int should_fail_bio(struct bio *bio)
{
	if (should_fail_request(&bio->bi_disk->part0, bio->bi_iter.bi_size))
		return -EIO;
	return 0;
}