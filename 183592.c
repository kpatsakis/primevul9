	__releases(rcu)
{
	struct igmp_mcf_iter_state *state = igmp_mcf_seq_private(seq);
	if (likely(state->im)) {
		spin_unlock_bh(&state->im->lock);
		state->im = NULL;
	}
	state->idev = NULL;
	state->dev = NULL;
	rcu_read_unlock();
}