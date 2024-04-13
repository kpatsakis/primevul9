R_API void r_core_anal_stats_free(RCoreAnalStats *s) {
	if (s) {
		free (s->block);
	}
	free (s);
}