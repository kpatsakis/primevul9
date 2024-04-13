regional_log_stats(struct regional *r)
{
	/* some basic assertions put here (non time critical code) */
	log_assert(ALIGNMENT >= sizeof(char*));
	log_assert(REGIONAL_CHUNK_SIZE > ALIGNMENT);
	log_assert(REGIONAL_CHUNK_SIZE-ALIGNMENT > REGIONAL_LARGE_OBJECT_SIZE);
	log_assert(REGIONAL_CHUNK_SIZE >= sizeof(struct regional));
	/* debug print */
	log_info("regional %u chunks, %u large",
		(unsigned)count_chunks(r), (unsigned)count_large(r));
}