file_error_core(struct magic_set *ms, int error, const char *f, va_list va,
    size_t lineno)
{
	char *buf = NULL;
	
	/* Only the first error is ok */
	if (ms->event_flags & EVENT_HAD_ERR)
		return;
	if (lineno != 0) {
		efree(ms->o.buf);
		ms->o.buf = NULL;
		file_printf(ms, "line %" SIZE_T_FORMAT "u: ", lineno);
	}

	vspprintf(&buf, 0, f, va);
	va_end(va);
	
	if (error > 0) {
		file_printf(ms, "%s (%s)", (*buf ? buf : ""), strerror(error));
	} else if (*buf) {
		file_printf(ms, "%s", buf);
	}
	
	if (buf) {
		efree(buf);
	}

	ms->event_flags |= EVENT_HAD_ERR;
	ms->error = error;
}