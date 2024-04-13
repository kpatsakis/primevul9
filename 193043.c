static const struct k_clock *clockid_to_kclock(const clockid_t id)
{
	if (id < 0)
		return (id & CLOCKFD_MASK) == CLOCKFD ?
			&clock_posix_dynamic : &clock_posix_cpu;

	if (id >= ARRAY_SIZE(posix_clocks) || !posix_clocks[id])
		return NULL;
	return posix_clocks[id];
}