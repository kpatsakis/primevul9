file_check_mem(struct magic_set *ms, unsigned int level)
{
	size_t len;

	if (level >= ms->c.len) {
		len = (ms->c.len += 20 + level) * sizeof(*ms->c.li);
		ms->c.li = CAST(struct level_info *, (ms->c.li == NULL) ?
		    emalloc(len) :
		    erealloc(ms->c.li, len));
		if (ms->c.li == NULL) {
			file_oomem(ms, len);
			return -1;
		}
	}
	ms->c.li[level].got_match = 0;
#ifdef ENABLE_CONDITIONALS
	ms->c.li[level].last_match = 0;
	ms->c.li[level].last_cond = COND_NONE;
#endif /* ENABLE_CONDITIONALS */
	return 0;
}