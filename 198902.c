enum regex_type filter_parse_regex(char *buff, int len, char **search, int *not)
{
	int type = MATCH_FULL;
	int i;

	if (buff[0] == '!') {
		*not = 1;
		buff++;
		len--;
	} else
		*not = 0;

	*search = buff;

	if (isdigit(buff[0]))
		return MATCH_INDEX;

	for (i = 0; i < len; i++) {
		if (buff[i] == '*') {
			if (!i) {
				type = MATCH_END_ONLY;
			} else if (i == len - 1) {
				if (type == MATCH_END_ONLY)
					type = MATCH_MIDDLE_ONLY;
				else
					type = MATCH_FRONT_ONLY;
				buff[i] = 0;
				break;
			} else {	/* pattern continues, use full glob */
				return MATCH_GLOB;
			}
		} else if (strchr("[?\\", buff[i])) {
			return MATCH_GLOB;
		}
	}
	if (buff[0] == '*')
		*search = buff + 1;

	return type;
}