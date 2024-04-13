static void scrub_submit(struct scrub_ctx *sctx)
{
	struct scrub_bio *sbio;

	if (sctx->curr == -1)
		return;

	sbio = sctx->bios[sctx->curr];
	sctx->curr = -1;
	scrub_pending_bio_inc(sctx);
	btrfsic_submit_bio(sbio->bio);
}