static int __init early_debug_pagealloc(char *buf)
{
	if (!buf)
		return -EINVAL;
	return kstrtobool(buf, &_debug_pagealloc_enabled);
}