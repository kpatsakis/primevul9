static bool f2fs_bio_post_read_required(struct bio *bio)
{
	return bio->bi_private && !bio->bi_status;
}