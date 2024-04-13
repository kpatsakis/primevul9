static stf_status unexpected(struct state *st, struct msg_digest *md UNUSED)
{
	loglog(RC_LOG_SERIOUS, "unexpected message received in state %s",
	       st->st_state->name);
	return STF_IGNORE;
}