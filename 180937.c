static int __init debug_guardpage_minorder_setup(char *buf)
{
	unsigned long res;

	if (kstrtoul(buf, 10, &res) < 0 ||  res > MAX_ORDER / 2) {
		pr_err("Bad debug_guardpage_minorder value\n");
		return 0;
	}
	_debug_guardpage_minorder = res;
	pr_info("Setting debug_guardpage_minorder to %lu\n", res);
	return 0;
}