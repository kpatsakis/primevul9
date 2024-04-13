static void parse_path(char *p, char *q, char *id, int idsz)
{
	struct SYMBOL *s;
	char *r, *op = NULL, *width;
	int i, fill, npar = 0;

	r = strstr(p, "class=\"");
	if (!r || r > q)
		return;
	r += 7;
	fill = strncmp(r, "fill", 4) == 0;
	width = strstr(p, "stroke-width:");
	for (;;) {
		p = strstr(p, "d=\"");
		if (!p)
			return;
		if (isspace((unsigned char) p[-1]))	// (check not 'id=..")
			break;
		p += 3;
	}
	r = tex_buf;
	*r++ = '/';
	idsz -= 5;
	strncpy(r, id + 4, idsz);
	r += idsz;
	*r++ = '{';
	*r++ = 'M';
	if (width && width < q) {
		*r++ = ' ';
		width += 13;
		while (isdigit(*width) || *width == '.')
			*r++ = *width++;
		*r++ = ' ';
		*r++ = 'S';
		*r++ = 'L';
		*r++ = 'W';
	}
	p += 3;
	for (;;) {
		if (*p == '\0' || *p == '"')
			break;
		switch (*p++) {
		default:
			if ((isdigit((unsigned char) p[-1]))
			 || p[-1] == '-' || p[-1] == '.') {
				if (!npar)
					continue;
				p--;			// same op
				break;
			}
			continue;
		case 'M':
		case 'm':
			op = "RM";
			npar = 2;
			break;
//		case 'L':
//			op = "L";
//			npar = 2;
//			break;
		case 'l':
			op = "RL";
			npar = 2;
			break;
//		case 'H':
//			op = "H";
//			npar = 1;
//			break;
		case 'h':
			op = "h";
			npar = 1;
			break;
//		case 'V':
//			op = "V";
//			npar = 1;
//			break;
		case 'v':
			*r++ = ' ';
			*r++ = '0';
			op = "RL";
			npar = 1;
			break;
		case 'z':
			op = "closepath";
			npar = 0;
			break;
//		case 'C':
//			op = "C";
//			npar = 6;
//			break;
		case 'c':
			op = "RC";
			npar = 6;
			break;
//		case 'A':
//			op = "arc";
//			break;
//		case 'a':
//			op = "arc";
//			break;
		}
		*r++ = ' ';
		for (i = 0; i < npar; i++) {
			while (isspace((unsigned char) *p))
				p++;
			if (i & 1) {		// y is inverted
				if (*p == '-')
					p++;
				else if (*p != '0' || p[1] != ' ')
					*r++ = '-';
			}
			while ((isdigit((unsigned char) *p))
			    || *p == '-' || *p == '.')
				*r++ = *p++;
			*r++ = ' ';
		}
		if (*op == 'h') {
			*r++ = '0';
			*r++ = ' ';
			op = "RL";
		}
		strcpy(r, op);
		r += strlen(r);
	}
	strcpy(r, fill ? " fill}!" : " stroke}!");
	s = getarena(sizeof(struct SYMBOL));
	memset(s, 0, sizeof(struct SYMBOL));
	s->text = getarena(strlen(tex_buf) + 1);
	strcpy(s->text, tex_buf);
	ps_def(s, s->text, 'p');
}