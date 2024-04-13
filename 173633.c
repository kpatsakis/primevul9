static int check_header(struct SYMBOL *s)
{
	switch (s->text[2]) {
	case 'E':
		if (strncmp(s->text + 2, "EPS", 3) == 0)
			return 0;
		break;
	case 'm':
		if (strncmp(s->text + 2, "multicol", 8) == 0)
			return 0;
		break;
	}
	return 1;
}