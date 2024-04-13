static int is_tune_sig(void)
{
	struct SYMBOL *s;

	if (!curvoice->sym)
		return 1;
	if (curvoice->time != 0)
		return 0;		/* not at start of tune */
	for (s = curvoice->sym; s; s = s->next) {
		switch (s->type) {
		case TEMPO:
		case PART:
		case FMTCHG:
			break;
		default:
			return 0;
		}
	}
	return 1;
}