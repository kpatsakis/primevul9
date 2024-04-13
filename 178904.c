static void del_chan(struct pppox_sock *sock)
{
	spin_lock(&chan_lock);
	clear_bit(sock->proto.pptp.src_addr.call_id, callid_bitmap);
	RCU_INIT_POINTER(callid_sock[sock->proto.pptp.src_addr.call_id], NULL);
	spin_unlock(&chan_lock);
	synchronize_rcu();
}