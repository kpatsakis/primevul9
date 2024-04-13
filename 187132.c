static int nfs4_delay(struct rpc_clnt *clnt, long *timeout)
{
	int res = 0;

	might_sleep();

	if (*timeout <= 0)
		*timeout = NFS4_POLL_RETRY_MIN;
	if (*timeout > NFS4_POLL_RETRY_MAX)
		*timeout = NFS4_POLL_RETRY_MAX;
	freezable_schedule_timeout_killable(*timeout);
	if (fatal_signal_pending(current))
		res = -ERESTARTSYS;
	*timeout <<= 1;
	return res;
}