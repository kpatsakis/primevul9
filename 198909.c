int filter_assign_type(const char *type)
{
	if (strstr(type, "__data_loc") && strstr(type, "char"))
		return FILTER_DYN_STRING;

	if (strchr(type, '[') && strstr(type, "char"))
		return FILTER_STATIC_STRING;

	if (strcmp(type, "char *") == 0 || strcmp(type, "const char *") == 0)
		return FILTER_PTR_STRING;

	return FILTER_OTHER;
}