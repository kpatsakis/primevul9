static int dccp_getsockopt_service(struct sock *sk, int len,
				   __be32 __user *optval,
				   int __user *optlen)
{
	const struct dccp_sock *dp = dccp_sk(sk);
	const struct dccp_service_list *sl;
	int err = -ENOENT, slen = 0, total_len = sizeof(u32);

	lock_sock(sk);
	if ((sl = dp->dccps_service_list) != NULL) {
		slen = sl->dccpsl_nr * sizeof(u32);
		total_len += slen;
	}

	err = -EINVAL;
	if (total_len > len)
		goto out;

	err = 0;
	if (put_user(total_len, optlen) ||
	    put_user(dp->dccps_service, optval) ||
	    (sl != NULL && copy_to_user(optval + 1, sl->dccpsl_list, slen)))
		err = -EFAULT;
out:
	release_sock(sk);
	return err;
}