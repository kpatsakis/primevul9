static void timelib_skip_day_suffix(char **ptr)
{
	if (isspace(**ptr)) {
		return;
	}
	if (!strncasecmp(*ptr, "nd", 2) || !strncasecmp(*ptr, "rd", 2) ||!strncasecmp(*ptr, "st", 2) || !strncasecmp(*ptr, "th", 2)) {
		*ptr += 2;
	}
}