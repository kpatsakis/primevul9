void init_timer_on_stack_key(struct timer_list *timer,
			     void (*func)(struct timer_list *),
			     unsigned int flags,
			     const char *name, struct lock_class_key *key)
{
	debug_object_init_on_stack(timer, &timer_debug_descr);
	do_init_timer(timer, func, flags, name, key);
}