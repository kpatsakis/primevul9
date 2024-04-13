log_tr(const char *fmt, ...)
{
    static FILE *fd_tr = NULL;
    static proftime_T start;
    proftime_T now;
    va_list ap;

    if (fd_tr == NULL)
    {
	fd_tr = fopen("termresponse.log", "w");
	profile_start(&start);
    }
    now = start;
    profile_end(&now);
    fprintf(fd_tr, "%s: %s ", profile_msg(&now),
					must_redraw == NOT_VALID ? "NV"
					: must_redraw == CLEAR ? "CL" : "  ");
    va_start(ap, fmt);
    vfprintf(fd_tr, fmt, ap);
    va_end(ap);
    fputc('\n', fd_tr);
    fflush(fd_tr);
}