int hwpoison_filter(struct page *p)
{
	if (!hwpoison_filter_enable)
		return 0;

	if (hwpoison_filter_dev(p))
		return -EINVAL;

	if (hwpoison_filter_flags(p))
		return -EINVAL;

	if (hwpoison_filter_task(p))
		return -EINVAL;

	return 0;
}