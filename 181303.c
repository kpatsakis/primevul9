file_reset(struct magic_set *ms)
{
	if (ms->mlist[0] == NULL) {
		file_error(ms, 0, "no magic files loaded");
		return -1;
	}
	if (ms->o.buf) {
		efree(ms->o.buf);
		ms->o.buf = NULL;
	}
	if (ms->o.pbuf) {
		efree(ms->o.pbuf);
		ms->o.pbuf = NULL;
	}
	ms->event_flags &= ~EVENT_HAD_ERR;
	ms->error = -1;
	return 0;
}