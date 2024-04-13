imapx_uidset_init (struct _uidset_state *ss,
                   gint total,
                   gint limit)
{
	ss->uids = 0;
	ss->entries = 0;
	ss->start = 0;
	ss->last = 0;
	ss->total = total;
	ss->limit = limit;
}