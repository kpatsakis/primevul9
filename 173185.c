static int bnx2x_check_firmware(struct bnx2x *bp)
{
	const struct firmware *firmware = bp->firmware;
	struct bnx2x_fw_file_hdr *fw_hdr;
	struct bnx2x_fw_file_section *sections;
	u32 offset, len, num_ops;
	__be16 *ops_offsets;
	int i;
	const u8 *fw_ver;

	if (firmware->size < sizeof(struct bnx2x_fw_file_hdr)) {
		BNX2X_ERR("Wrong FW size\n");
		return -EINVAL;
	}

	fw_hdr = (struct bnx2x_fw_file_hdr *)firmware->data;
	sections = (struct bnx2x_fw_file_section *)fw_hdr;

	/* Make sure none of the offsets and sizes make us read beyond
	 * the end of the firmware data */
	for (i = 0; i < sizeof(*fw_hdr) / sizeof(*sections); i++) {
		offset = be32_to_cpu(sections[i].offset);
		len = be32_to_cpu(sections[i].len);
		if (offset + len > firmware->size) {
			BNX2X_ERR("Section %d length is out of bounds\n", i);
			return -EINVAL;
		}
	}

	/* Likewise for the init_ops offsets */
	offset = be32_to_cpu(fw_hdr->init_ops_offsets.offset);
	ops_offsets = (__force __be16 *)(firmware->data + offset);
	num_ops = be32_to_cpu(fw_hdr->init_ops.len) / sizeof(struct raw_op);

	for (i = 0; i < be32_to_cpu(fw_hdr->init_ops_offsets.len) / 2; i++) {
		if (be16_to_cpu(ops_offsets[i]) > num_ops) {
			BNX2X_ERR("Section offset %d is out of bounds\n", i);
			return -EINVAL;
		}
	}

	/* Check FW version */
	offset = be32_to_cpu(fw_hdr->fw_version.offset);
	fw_ver = firmware->data + offset;
	if ((fw_ver[0] != BCM_5710_FW_MAJOR_VERSION) ||
	    (fw_ver[1] != BCM_5710_FW_MINOR_VERSION) ||
	    (fw_ver[2] != BCM_5710_FW_REVISION_VERSION) ||
	    (fw_ver[3] != BCM_5710_FW_ENGINEERING_VERSION)) {
		BNX2X_ERR("Bad FW version:%d.%d.%d.%d. Should be %d.%d.%d.%d\n",
		       fw_ver[0], fw_ver[1], fw_ver[2], fw_ver[3],
		       BCM_5710_FW_MAJOR_VERSION,
		       BCM_5710_FW_MINOR_VERSION,
		       BCM_5710_FW_REVISION_VERSION,
		       BCM_5710_FW_ENGINEERING_VERSION);
		return -EINVAL;
	}

	return 0;
}