removeSuffix(char *string, const char *suffix)
{
	size_t stringLength = strlen(string);
	size_t suffixLength = strlen(suffix);

	if (stringLength >= suffixLength) {
		char *tail = &string[stringLength - suffixLength];

		if (0 == strcmp(tail, suffix)) {
			*tail = '\0';
		}
	}
}