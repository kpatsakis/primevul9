static void ipq_kill(struct ipq *ipq)
{
	inet_frag_kill(&ipq->q);
}