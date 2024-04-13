static int bnx2x_init_firmware(struct bnx2x *bp)
{
	const char *fw_file_name;
	struct bnx2x_fw_file_hdr *fw_hdr;
	int rc;

	if (bp->firmware)
		return 0;

	if (CHIP_IS_E1(bp))
		fw_file_name = FW_FILE_NAME_E1;
	else if (CHIP_IS_E1H(bp))
		fw_file_name = FW_FILE_NAME_E1H;
	else if (!CHIP_IS_E1x(bp))
		fw_file_name = FW_FILE_NAME_E2;
	else {
		BNX2X_ERR("Unsupported chip revision\n");
		return -EINVAL;
	}
	BNX2X_DEV_INFO("Loading %s\n", fw_file_name);

	rc = request_firmware(&bp->firmware, fw_file_name, &bp->pdev->dev);
	if (rc) {
		BNX2X_ERR("Can't load firmware file %s\n",
			  fw_file_name);
		goto request_firmware_exit;
	}

	rc = bnx2x_check_firmware(bp);
	if (rc) {
		BNX2X_ERR("Corrupt firmware file %s\n", fw_file_name);
		goto request_firmware_exit;
	}

	fw_hdr = (struct bnx2x_fw_file_hdr *)bp->firmware->data;

	/* Initialize the pointers to the init arrays */
	/* Blob */
	rc = -ENOMEM;
	BNX2X_ALLOC_AND_SET(init_data, request_firmware_exit, be32_to_cpu_n);

	/* Opcodes */
	BNX2X_ALLOC_AND_SET(init_ops, init_ops_alloc_err, bnx2x_prep_ops);

	/* Offsets */
	BNX2X_ALLOC_AND_SET(init_ops_offsets, init_offsets_alloc_err,
			    be16_to_cpu_n);

	/* STORMs firmware */
	INIT_TSEM_INT_TABLE_DATA(bp) = bp->firmware->data +
			be32_to_cpu(fw_hdr->tsem_int_table_data.offset);
	INIT_TSEM_PRAM_DATA(bp)      = bp->firmware->data +
			be32_to_cpu(fw_hdr->tsem_pram_data.offset);
	INIT_USEM_INT_TABLE_DATA(bp) = bp->firmware->data +
			be32_to_cpu(fw_hdr->usem_int_table_data.offset);
	INIT_USEM_PRAM_DATA(bp)      = bp->firmware->data +
			be32_to_cpu(fw_hdr->usem_pram_data.offset);
	INIT_XSEM_INT_TABLE_DATA(bp) = bp->firmware->data +
			be32_to_cpu(fw_hdr->xsem_int_table_data.offset);
	INIT_XSEM_PRAM_DATA(bp)      = bp->firmware->data +
			be32_to_cpu(fw_hdr->xsem_pram_data.offset);
	INIT_CSEM_INT_TABLE_DATA(bp) = bp->firmware->data +
			be32_to_cpu(fw_hdr->csem_int_table_data.offset);
	INIT_CSEM_PRAM_DATA(bp)      = bp->firmware->data +
			be32_to_cpu(fw_hdr->csem_pram_data.offset);
	/* IRO */
	BNX2X_ALLOC_AND_SET(iro_arr, iro_alloc_err, bnx2x_prep_iro);

	return 0;

iro_alloc_err:
	kfree(bp->init_ops_offsets);
init_offsets_alloc_err:
	kfree(bp->init_ops);
init_ops_alloc_err:
	kfree(bp->init_data);
request_firmware_exit:
	release_firmware(bp->firmware);
	bp->firmware = NULL;

	return rc;
}