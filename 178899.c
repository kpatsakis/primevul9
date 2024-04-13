static int add_chan(struct pppox_sock *sock)
{
	static int call_id;

	spin_lock(&chan_lock);
	if (!sock->proto.pptp.src_addr.call_id)	{
		call_id = find_next_zero_bit(callid_bitmap, MAX_CALLID, call_id + 1);
		if (call_id == MAX_CALLID) {
			call_id = find_next_zero_bit(callid_bitmap, MAX_CALLID, 1);
			if (call_id == MAX_CALLID)
				goto out_err;
		}
		sock->proto.pptp.src_addr.call_id = call_id;
	} else if (test_bit(sock->proto.pptp.src_addr.call_id, callid_bitmap))
		goto out_err;

	set_bit(sock->proto.pptp.src_addr.call_id, callid_bitmap);
	rcu_assign_pointer(callid_sock[sock->proto.pptp.src_addr.call_id], sock);
	spin_unlock(&chan_lock);

	return 0;

out_err:
	spin_unlock(&chan_lock);
	return -1;
}