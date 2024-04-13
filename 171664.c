static void sk_leave_memory_pressure(struct sock *sk)
{
	if (sk->sk_prot->leave_memory_pressure) {
		sk->sk_prot->leave_memory_pressure(sk);
	} else {
		unsigned long *memory_pressure = sk->sk_prot->memory_pressure;

		if (memory_pressure && *memory_pressure)
			*memory_pressure = 0;
	}
}