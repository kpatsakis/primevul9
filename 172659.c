static void scrub_parity_bio_endio_worker(struct btrfs_work *work)
{
	struct scrub_parity *sparity = container_of(work, struct scrub_parity,
						    work);
	struct scrub_ctx *sctx = sparity->sctx;

	scrub_free_parity(sparity);
	scrub_pending_bio_dec(sctx);
}