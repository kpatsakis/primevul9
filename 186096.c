void __init unittest_unflatten_overlay_base(void)
{
	struct overlay_info *info;
	u32 data_size;
	void *new_fdt;
	u32 size;
	int found = 0;
	const char *overlay_name = "overlay_base";

	for (info = overlays; info && info->name; info++) {
		if (!strcmp(overlay_name, info->name)) {
			found = 1;
			break;
		}
	}
	if (!found) {
		pr_err("no overlay data for %s\n", overlay_name);
		return;
	}

	info = &overlays[0];

	if (info->expected_result != -9999) {
		pr_err("No dtb 'overlay_base' to attach\n");
		return;
	}

	data_size = info->dtb_end - info->dtb_begin;
	if (!data_size) {
		pr_err("No dtb 'overlay_base' to attach\n");
		return;
	}

	size = fdt_totalsize(info->dtb_begin);
	if (size != data_size) {
		pr_err("dtb 'overlay_base' header totalsize != actual size");
		return;
	}

	new_fdt = dt_alloc_memory(size, roundup_pow_of_two(FDT_V17_SIZE));
	if (!new_fdt) {
		pr_err("alloc for dtb 'overlay_base' failed");
		return;
	}

	memcpy(new_fdt, info->dtb_begin, size);

	__unflatten_device_tree(new_fdt, NULL, &overlay_base_root,
				dt_alloc_memory, true);
}