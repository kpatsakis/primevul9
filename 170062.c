static timelib_sll timelib_lookup_relative_text(char **ptr, int *behavior)
{
	char *word;
	char *begin = *ptr, *end;
	timelib_sll  value = 0;
	const timelib_lookup_table *tp;

	while ((**ptr >= 'A' && **ptr <= 'Z') || (**ptr >= 'a' && **ptr <= 'z')) {
		++*ptr;
	}
	end = *ptr;
	word = timelib_calloc(1, end - begin + 1);
	memcpy(word, begin, end - begin);

	for (tp = timelib_reltext_lookup; tp->name; tp++) {
		if (strcasecmp(word, tp->name) == 0) {
			value = tp->value;
			*behavior = tp->type;
		}
	}

	timelib_free(word);
	return value;
}