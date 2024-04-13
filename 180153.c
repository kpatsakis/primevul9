compare_class_imap (const void *key, const void *elem)
{
	const char** class_name = (const char**)elem;
	return strcmp (key, *class_name);
}