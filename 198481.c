util_get_pin(const char *input, const char **pin)
{
	size_t inputlen = strlen(input);
	size_t pinlen = 0;

	if(inputlen > 4 && strncasecmp(input, "env:", 4) == 0) {
		// Get a PIN from a environment variable
		*pin = getenv(input + 4);
		pinlen = *pin ? strlen(*pin) : 0;
	} else {
		//Just use the input
		*pin = input;
		pinlen = inputlen;
	}
	return pinlen;
}