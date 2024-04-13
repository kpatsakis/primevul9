static char *get_symsel(struct symsel_s *symsel, char *p)
{
	char *q;
	int tn, td, n;

	symsel->bar = strtod(p, &q);
	if (*q >= 'a' && *q <= 'z')
		symsel->seq = *q++ - 'a';
	else
		symsel->seq = 0;
	if (*q == ':') {
		if (sscanf(q + 1, "%d/%d%n", &tn, &td, &n) != 2
		 || td <= 0)
			return 0;
		symsel->time = BASE_LEN * tn / td;
		q += 1 + n;
	} else {
		symsel->time = 0;
	}
	return q;
}