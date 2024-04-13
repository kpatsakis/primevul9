imapx_uidset_done (struct _uidset_state *ss,
                   CamelIMAPXCommand *ic)
{
	gint ret = FALSE;

	if (ss->last != 0) {
		if (ss->entries > 0)
			camel_imapx_command_add (ic, ",");
		if (ss->last == ss->start)
			camel_imapx_command_add (ic, "%d", ss->last);
		else
			camel_imapx_command_add (ic, "%d:%d", ss->start, ss->last);
	}

	ret = ss->last != 0;

	ss->start = 0;
	ss->last = 0;
	ss->uids = 0;
	ss->entries = 0;

	return ret;
}