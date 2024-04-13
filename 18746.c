static void success_message(struct libmnt_context *cxt)
{
	unsigned long mflags = 0;
	const char *tgt, *src;

	if (mnt_context_helper_executed(cxt)
	    || mnt_context_get_status(cxt) != 1)
		return;

	mnt_context_get_mflags(cxt, &mflags);
	tgt = mnt_context_get_target(cxt);
	src = mnt_context_get_source(cxt);

	if (mflags & MS_MOVE)
		warnx(_("%s moved to %s"), src, tgt);
	else if (mflags & MS_BIND)
		warnx(_("%s binded on %s"), src, tgt);
	else if (mflags & MS_PROPAGATION)
		warnx(_("%s propagation flags changed"), tgt);
	else
		warnx(_("%s mounted on %s"), src, tgt);
}