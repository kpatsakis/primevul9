static noinline_for_stack void scrub_free_ctx(struct scrub_ctx *sctx)
{
	int i;

	if (!sctx)
		return;

	/* this can happen when scrub is cancelled */
	if (sctx->curr != -1) {
		struct scrub_bio *sbio = sctx->bios[sctx->curr];

		for (i = 0; i < sbio->page_count; i++) {
			WARN_ON(!sbio->pagev[i]->page);
			scrub_block_put(sbio->pagev[i]->sblock);
		}
		bio_put(sbio->bio);
	}

	for (i = 0; i < SCRUB_BIOS_PER_SCTX; ++i) {
		struct scrub_bio *sbio = sctx->bios[i];

		if (!sbio)
			break;
		kfree(sbio);
	}

	kfree(sctx->wr_curr_bio);
	scrub_free_csums(sctx);
	kfree(sctx);
}