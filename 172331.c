imapx_uidset_add (struct _uidset_state *ss,
                  CamelIMAPXCommand *ic,
                  const gchar *uid)
{
	guint32 uidn;

	uidn = strtoul (uid, NULL, 10);
	if (uidn == 0)
		return -1;

	ss->uids++;

	e (ic->is->priv->tagprefix, "uidset add '%s'\n", uid);

	if (ss->last == 0) {
		e (ic->is->priv->tagprefix, " start\n");
		ss->start = uidn;
		ss->last = uidn;
	} else {
		if (ss->start - 1 == uidn) {
			ss->start = uidn;
		} else {
			if (ss->last != uidn - 1) {
				if (ss->last == ss->start) {
					e (ic->is->priv->tagprefix, " ,next\n");
					if (ss->entries > 0)
						camel_imapx_command_add (ic, ",");
					camel_imapx_command_add (ic, "%u", ss->start);
					ss->entries++;
				} else {
					e (ic->is->priv->tagprefix, " :range\n");
					if (ss->entries > 0)
						camel_imapx_command_add (ic, ",");
					camel_imapx_command_add (ic, "%u:%u", ss->start, ss->last);
					ss->entries += 2;
				}
				ss->start = uidn;
			}

			ss->last = uidn;
		}
	}

	if ((ss->limit && ss->entries >= ss->limit)
	    || (ss->limit && ss->uids >= ss->limit)
	    || (ss->total && ss->uids >= ss->total)) {
		e (ic->is->priv->tagprefix, " done, %d entries, %d uids\n", ss->entries, ss->uids);
		if (!imapx_uidset_done (ss, ic))
			return -1;
		return 1;
	}

	return 0;
}