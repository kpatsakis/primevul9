static void scrub_bio_wait_endio(struct bio *bio)
{
	complete(bio->bi_private);
}