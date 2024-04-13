static __init int activate_jump_labels(void)
{
	if (has_steal_clock) {
		static_key_slow_inc(&paravirt_steal_enabled);
		if (steal_acc)
			static_key_slow_inc(&paravirt_steal_rq_enabled);
	}

	return 0;
}