compare_method_imap (const void *key, const void *elem)
{
	const char** method_name = (const char**)elem;
	return strcmp (key, *method_name);
}