static void scrub_pending_bio_inc(struct scrub_ctx *sctx)
{
	refcount_inc(&sctx->refs);
	atomic_inc(&sctx->bios_in_flight);
}