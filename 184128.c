void untty(void)
#ifdef HAVE_SETSID
{
	if(!Debug) {
		setsid();
	}
	return;
}