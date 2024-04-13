static void __init of_unittest_overlay_6(void)
{
	int i, ov_id[2], ovcs_id;
	int overlay_nr = 6, unittest_nr = 6;
	int before = 0, after = 1;
	const char *overlay_name;

	/* unittest device must be in before state */
	for (i = 0; i < 2; i++) {
		if (of_unittest_device_exists(unittest_nr + i, PDEV_OVERLAY)
				!= before) {
			unittest(0, "%s with device @\"%s\" %s\n",
					overlay_name_from_nr(overlay_nr + i),
					unittest_path(unittest_nr + i,
						PDEV_OVERLAY),
					!before ? "enabled" : "disabled");
			return;
		}
	}

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

	for (i = 0; i < 2; i++) {
		/* unittest device must be in after state */
		if (of_unittest_device_exists(unittest_nr + i, PDEV_OVERLAY)
				!= after) {
			unittest(0, "overlay @\"%s\" failed @\"%s\" %s\n",
					overlay_name_from_nr(overlay_nr + i),
					unittest_path(unittest_nr + i,
						PDEV_OVERLAY),
					!after ? "enabled" : "disabled");
			return;
		}
	}

	for (i = 1; i >= 0; i--) {
		ovcs_id = ov_id[i];
		if (of_overlay_remove(&ovcs_id)) {
			unittest(0, "%s failed destroy @\"%s\"\n",
					overlay_name_from_nr(overlay_nr + i),
					unittest_path(unittest_nr + i,
						PDEV_OVERLAY));
			return;
		}
		of_unittest_untrack_overlay(ov_id[i]);
	}

	for (i = 0; i < 2; i++) {
		/* unittest device must be again in before state */
		if (of_unittest_device_exists(unittest_nr + i, PDEV_OVERLAY)
				!= before) {
			unittest(0, "%s with device @\"%s\" %s\n",
					overlay_name_from_nr(overlay_nr + i),
					unittest_path(unittest_nr + i,
						PDEV_OVERLAY),
					!before ? "enabled" : "disabled");
			return;
		}
	}

	unittest(1, "overlay test %d passed\n", 6);
}