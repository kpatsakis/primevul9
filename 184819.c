static void ipq_put(struct ipq *ipq)
{
	inet_frag_put(&ipq->q);
}