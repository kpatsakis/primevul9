spool_pname_buf(uschar * buf, int len)
{
snprintf(CS buf, len, "%s/%s/input", spool_directory, queue_name);
}