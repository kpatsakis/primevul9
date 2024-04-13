void dccp_hash(struct sock *sk)
{
	inet_hash(&dccp_hashinfo, sk);
}