static void put_pdfmark(char *p)
{
	unsigned char c, *q;
	int u;

	p = trim_title(p, NULL);

	/* check if pure ASCII without '\', '(' nor ')'*/
	for (q = (unsigned char *) p; *q != '\0'; q++) {
		switch (*q) {
		case '\\':
		case '(':
		case ')':
			break;
		default:
			if (*q >= 0x80)
				break;
			continue;
		}
		break;
	}
	if (*q == '\0') {
		a2b("[/Title(%s)/OUT pdfmark\n", p);
		return;
	}

	/* build utf-8 mark */
	a2b("[/Title<FEFF");
	q = (unsigned char *) p;
	u = -1;
	while (*q != '\0') {
		c = *q++;
		if (c < 0x80) {
			if (u >= 0) {
				a2b("%04X", u);
				u = -1;
			}
			a2b("%04X", (int) c);
			continue;
		}
		if (c < 0xc0) {
			u = (u << 6) | (c & 0x3f);
			continue;
		}
		if (u >= 0) {
			a2b("%04X", u);
			u = -1;
		}
		if (c < 0xe0)
			u = c & 0x1f;
		else if (c < 0xf0)
			u = c & 0x0f;
		else
			u = c & 0x07;
	}
	if (u >= 0) {
		a2b("%04X", u);
		u = -1;
	}
	a2b(">/OUT pdfmark\n");
}