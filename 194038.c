static inline bool bio_integrity_endio(struct bio *bio)
{
	if (bio_integrity(bio))
		return __bio_integrity_endio(bio);
	return true;
}