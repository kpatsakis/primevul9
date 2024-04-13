static bool block_flags_stat(RFlagItem *fi, void *user) {
	struct block_flags_stat_t *u = (struct block_flags_stat_t *)user;
	int piece = (fi->offset - u->from) / u->step;
	u->as->block[piece].flags++;
	return true;
}