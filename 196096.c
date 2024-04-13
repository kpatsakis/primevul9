spool_sname(const uschar * purpose, uschar * subdir)
{
return string_sprintf("%s%s%s%s%s",
		    queue_name, *queue_name ? "/" : "",
		    purpose,
		    *subdir ? "/" : "", subdir);
}