static inline struct bio *f2fs_bio_alloc(struct f2fs_sb_info *sbi,
						int npages, bool no_fail)
{
	struct bio *bio;

	if (no_fail) {
		/* No failure on bio allocation */
		bio = bio_alloc(GFP_NOIO, npages);
		if (!bio)
			bio = bio_alloc(GFP_NOIO | __GFP_NOFAIL, npages);
		return bio;
	}
	if (time_to_inject(sbi, FAULT_ALLOC_BIO)) {
		f2fs_show_injection_info(FAULT_ALLOC_BIO);
		return NULL;
	}

	return bio_alloc(GFP_KERNEL, npages);
}