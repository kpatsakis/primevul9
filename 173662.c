static void gch_transpose(struct SYMBOL *s)
{
	int i2 = ((curvoice->ckey.sf - curvoice->okey.sf + 12) * 7) % 12;
	char *o = s->text, *p = o, *q = o, *r;

	// search the chord symbols
	for (;;) {
		if (!strchr("^_<>@", *p)) {
			q = strchr(p, '\t');
			if (q) {
				r = strchr(p, '\n');
				if (!r || q < r)
					gch_tr1(s, q + 1 - o, i2);
			}
			gch_tr1(s, p - o, i2);
		}
		p = strchr(p, '\n');
		if (!p)
			return;
		p++;
	}
}