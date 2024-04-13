seamless_reset_state(void)
{
	if (seamless_rest != NULL)
	{
		xfree(seamless_rest);
		seamless_rest = NULL;
	}
}