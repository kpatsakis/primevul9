__weak void abort(void)
{
	BUG();

	/* if that doesn't kill us, halt */
	panic("Oops failed to kill thread");
}