void whiteSpace_replace(xmlChar* str)
{
	while (*str != '\0') {
		if (*str == '\x9' || *str == '\xA' || *str == '\xD') {
			*str = ' ';
		}
		str++;
	}
}