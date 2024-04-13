file_softmagic(struct magic_set *ms, const unsigned char *buf, size_t nbytes,
    uint16_t indir_level, uint16_t *name_count, int mode, int text)
{
	struct mlist *ml;
	int rv, printed_something = 0, need_separator = 0;
	uint16_t nc;

	if (name_count == NULL) {
		nc = 0;
		name_count = &nc;
	}

	for (ml = ms->mlist[0]->next; ml != ms->mlist[0]; ml = ml->next)
		if ((rv = match(ms, ml->magic, ml->nmagic, buf, nbytes, 0, mode,
		    text, 0, indir_level, name_count,
		    &printed_something, &need_separator, NULL)) != 0)
			return rv;

	return 0;
}