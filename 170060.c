static const timelib_relunit* timelib_lookup_relunit(char **ptr)
{
	char *word;
	char *begin = *ptr, *end;
	const timelib_relunit *tp, *value = NULL;

	while (**ptr != '\0' && **ptr != ' ' && **ptr != ',' && **ptr != '\t' && **ptr != ';' && **ptr != ':' &&
           **ptr != '/' && **ptr != '.' && **ptr != '-' && **ptr != '(' && **ptr != ')' ) {
		++*ptr;
	}
	end = *ptr;
	word = timelib_calloc(1, end - begin + 1);
	memcpy(word, begin, end - begin);

	for (tp = timelib_relunit_lookup; tp->name; tp++) {
		if (strcasecmp(word, tp->name) == 0) {
			value = tp;
			break;
		}
	}

	timelib_free(word);
	return value;
}