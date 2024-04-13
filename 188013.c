static void fanout_release(struct sock *sk)
{
	struct packet_sock *po = pkt_sk(sk);
	struct packet_fanout *f;

	f = po->fanout;
	if (!f)
		return;

	po->fanout = NULL;

	mutex_lock(&fanout_mutex);
	if (atomic_dec_and_test(&f->sk_ref)) {
		list_del(&f->list);
		dev_remove_pack(&f->prot_hook);
		kfree(f);
	}
	mutex_unlock(&fanout_mutex);
}