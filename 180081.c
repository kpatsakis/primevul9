compare_method_imap (const void *key, const void *elem)
{
	const char* method_name = (const char*)&icall_names_str + (*(guint16*)elem);
	return strcmp (key, method_name);
}