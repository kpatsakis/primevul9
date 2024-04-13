static inline void sock_valbool_flag(struct sock *sk, int bit, int valbool)
{
	if (valbool)
		sock_set_flag(sk, bit);
	else
		sock_reset_flag(sk, bit);
}