static int __init overlay_data_apply(const char *overlay_name, int *overlay_id)
{
	struct overlay_info *info;
	int found = 0;
	int ret;
	u32 size;

	for (info = overlays; info && info->name; info++) {
		if (!strcmp(overlay_name, info->name)) {
			found = 1;
			break;
		}
	}
	if (!found) {
		pr_err("no overlay data for %s\n", overlay_name);
		return 0;
	}

	size = info->dtb_end - info->dtb_begin;
	if (!size)
		pr_err("no overlay data for %s\n", overlay_name);

	ret = of_overlay_fdt_apply(info->dtb_begin, size, &info->overlay_id);
	if (overlay_id)
		*overlay_id = info->overlay_id;
	if (ret < 0)
		goto out;

	pr_debug("%s applied\n", overlay_name);

out:
	if (ret != info->expected_result)
		pr_err("of_overlay_fdt_apply() expected %d, ret=%d, %s\n",
		       info->expected_result, ret, overlay_name);

	return (ret == info->expected_result);
}