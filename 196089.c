spool_fname(const uschar * purpose, const uschar * subdir, const uschar * fname,
       	const uschar * suffix)
{
return string_sprintf("%s/%s/%s/%s/%s%s",
	spool_directory, queue_name, purpose, subdir, fname, suffix);
}