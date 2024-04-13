static void try_to_generate_entropy(void)
{
	struct {
		unsigned long now;
		struct timer_list timer;
	} stack;

	stack.now = random_get_entropy();

	/* Slow counter - or none. Don't even bother */
	if (stack.now == random_get_entropy())
		return;

	timer_setup_on_stack(&stack.timer, entropy_timer, 0);
	while (!crng_ready()) {
		if (!timer_pending(&stack.timer))
			mod_timer(&stack.timer, jiffies+1);
		mix_pool_bytes(&input_pool, &stack.now, sizeof(stack.now));
		schedule();
		stack.now = random_get_entropy();
	}

	del_timer_sync(&stack.timer);
	destroy_timer_on_stack(&stack.timer);
	mix_pool_bytes(&input_pool, &stack.now, sizeof(stack.now));
}