__rds_conn_path_error(struct rds_conn_path *cp, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintk(fmt, ap);
	va_end(ap);

	rds_conn_path_drop(cp, false);
}