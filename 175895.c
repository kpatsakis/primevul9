void whiteSpace_collapse(xmlChar* str)
{
	xmlChar *pos;
	xmlChar old;

	pos = str;
	whiteSpace_replace(str);
	while (*str == ' ') {
		str++;
	}
	old = '\0';
	while (*str != '\0') {
		if (*str != ' ' || old != ' ') {
			*pos = *str;
			pos++;
		}
		old = *str;
		str++;
	}
	if (old == ' ') {
	 	--pos;
	}
	*pos = '\0';
}