static void success_message(struct libmnt_context *cxt)
{
	const char *tgt, *src;

	if (mnt_context_helper_executed(cxt)
	    || mnt_context_get_status(cxt) != 1)
		return;

	tgt = mnt_context_get_target(cxt);
	if (!tgt)
		return;

	src = mnt_context_get_source(cxt);
	if (src)
		warnx(_("%s (%s) unmounted"), tgt, src);
	else
		warnx(_("%s unmounted"), tgt);
}