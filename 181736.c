static void aea_stats_init (AeaStats *stats) {
	stats->regs = r_list_newf (free);
	stats->regread = r_list_newf (free);
	stats->regwrite = r_list_newf (free);
	stats->regvalues = r_list_newf (free);
	stats->inputregs = r_list_newf (free);
}