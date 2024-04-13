static int dccp_setsockopt_service(struct sock *sk, const __be32 service,
				   char __user *optval, int optlen)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct dccp_service_list *sl = NULL;

	if (service == DCCP_SERVICE_INVALID_VALUE ||
	    optlen > DCCP_SERVICE_LIST_MAX_LEN * sizeof(u32))
		return -EINVAL;

	if (optlen > sizeof(service)) {
		sl = kmalloc(optlen, GFP_KERNEL);
		if (sl == NULL)
			return -ENOMEM;

		sl->dccpsl_nr = optlen / sizeof(u32) - 1;
		if (copy_from_user(sl->dccpsl_list,
				   optval + sizeof(service),
				   optlen - sizeof(service)) ||
		    dccp_list_has_service(sl, DCCP_SERVICE_INVALID_VALUE)) {
			kfree(sl);
			return -EFAULT;
		}
	}

	lock_sock(sk);
	dp->dccps_service = service;

	kfree(dp->dccps_service_list);

	dp->dccps_service_list = sl;
	release_sock(sk);
	return 0;
}