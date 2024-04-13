	__releases(rcu)
{
	struct igmp_mc_iter_state *state = igmp_mc_seq_private(seq);

	state->in_dev = NULL;
	state->dev = NULL;
	rcu_read_unlock();
}