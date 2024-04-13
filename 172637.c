static void scrub_free_parity(struct scrub_parity *sparity)
{
	struct scrub_ctx *sctx = sparity->sctx;
	struct scrub_page *curr, *next;
	int nbits;

	nbits = bitmap_weight(sparity->ebitmap, sparity->nsectors);
	if (nbits) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.read_errors += nbits;
		sctx->stat.uncorrectable_errors += nbits;
		spin_unlock(&sctx->stat_lock);
	}

	list_for_each_entry_safe(curr, next, &sparity->spages, list) {
		list_del_init(&curr->list);
		scrub_page_put(curr);
	}

	kfree(sparity);
}