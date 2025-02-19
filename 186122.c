static int __init of_unittest_apply_revert_overlay_check(int overlay_nr,
		int unittest_nr, int before, int after,
		enum overlay_type ovtype)
{
	int ret, ovcs_id;

	/* unittest device must be in before state */
	if (of_unittest_device_exists(unittest_nr, ovtype) != before) {
		unittest(0, "%s with device @\"%s\" %s\n",
				overlay_name_from_nr(overlay_nr),
				unittest_path(unittest_nr, ovtype),
				!before ? "enabled" : "disabled");
		return -EINVAL;
	}

	/* apply the overlay */
	ovcs_id = 0;
	ret = of_unittest_apply_overlay(overlay_nr, &ovcs_id);
	if (ret != 0) {
		/* of_unittest_apply_overlay already called unittest() */
		return ret;
	}

	/* unittest device must be in after state */
	if (of_unittest_device_exists(unittest_nr, ovtype) != after) {
		unittest(0, "%s failed to create @\"%s\" %s\n",
				overlay_name_from_nr(overlay_nr),
				unittest_path(unittest_nr, ovtype),
				!after ? "enabled" : "disabled");
		return -EINVAL;
	}

	ret = of_overlay_remove(&ovcs_id);
	if (ret != 0) {
		unittest(0, "%s failed to be destroyed @\"%s\"\n",
				overlay_name_from_nr(overlay_nr),
				unittest_path(unittest_nr, ovtype));
		return ret;
	}

	/* unittest device must be again in before state */
	if (of_unittest_device_exists(unittest_nr, PDEV_OVERLAY) != before) {
		unittest(0, "%s with device @\"%s\" %s\n",
				overlay_name_from_nr(overlay_nr),
				unittest_path(unittest_nr, ovtype),
				!before ? "enabled" : "disabled");
		return -EINVAL;
	}

	return 0;
}