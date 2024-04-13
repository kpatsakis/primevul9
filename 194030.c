static int __init elevator_setup(char *str)
{
	/*
	 * Be backwards-compatible with previous kernels, so users
	 * won't get the wrong elevator.
	 */
	strncpy(chosen_elevator, str, sizeof(chosen_elevator) - 1);
	return 1;
}