spool_dname(const uschar * purpose, uschar * subdir)
{
return string_sprintf("%s/%s/%s/%s",
	spool_directory, queue_name, purpose, subdir);
}