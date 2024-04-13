static void __init of_unittest_overlay_8(void)
{
	int i, ov_id[2], ovcs_id;
	int overlay_nr = 8, unittest_nr = 8;
	const char *overlay_name;

	/* we don't care about device state in this test */

	/* apply the overlays */
	for (i = 0; i < 2; i++) {

		overlay_name = overlay_name_from_nr(overlay_nr + i);

		if (!overlay_data_apply(overlay_name, &ovcs_id)) {
			unittest(0, "could not apply overlay \"%s\"\n",
					overlay_name);
			return;
		}
		ov_id[i] = ovcs_id;
		of_unittest_track_overlay(ov_id[i]);
	}

	/* now try to remove first overlay (it should fail) */
	ovcs_id = ov_id[0];
	if (!of_overlay_remove(&ovcs_id)) {
		unittest(0, "%s was destroyed @\"%s\"\n",
				overlay_name_from_nr(overlay_nr + 0),
				unittest_path(unittest_nr,
					PDEV_OVERLAY));
		return;
	}

	/* removing them in order should work */
	for (i = 1; i >= 0; i--) {
		ovcs_id = ov_id[i];
		if (of_overlay_remove(&ovcs_id)) {
			unittest(0, "%s not destroyed @\"%s\"\n",
					overlay_name_from_nr(overlay_nr + i),
					unittest_path(unittest_nr,
						PDEV_OVERLAY));
			return;
		}
		of_unittest_untrack_overlay(ov_id[i]);
	}

	unittest(1, "overlay test %d passed\n", 8);
}