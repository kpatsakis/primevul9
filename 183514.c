static inline void textdomain_init(void)
{
	if (!initialized) {
		bindtextdomain(MODULE_NAME, LOCALEDIR);
		initialized = 1;
	}
	return;
}