static inline struct packet_sock *pkt_sk(struct sock *sk)
{
	return (struct packet_sock *)sk;
}