static bool timer_fixup_activate(void *addr, enum debug_obj_state state)
{
	struct timer_list *timer = addr;

	switch (state) {
	case ODEBUG_STATE_NOTAVAILABLE:
		timer_setup(timer, stub_timer, 0);
		return true;

	case ODEBUG_STATE_ACTIVE:
		WARN_ON(1);
		/* fall through */
	default:
		return false;
	}
}