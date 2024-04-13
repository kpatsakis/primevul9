static int __init of_unittest_apply_overlay(int overlay_nr, int *overlay_id)
{
	const char *overlay_name;

	overlay_name = overlay_name_from_nr(overlay_nr);

	if (!overlay_data_apply(overlay_name, overlay_id)) {
		unittest(0, "could not apply overlay \"%s\"\n",
				overlay_name);
		return -EFAULT;
	}
	of_unittest_track_overlay(*overlay_id);

	return 0;
}