static void mwifiex_pcie_get_fw_name(struct mwifiex_adapter *adapter)
{
	int revision_id = 0;
	int version, magic;
	struct pcie_service_card *card = adapter->card;

	switch (card->dev->device) {
	case PCIE_DEVICE_ID_MARVELL_88W8766P:
		strcpy(adapter->fw_name, PCIE8766_DEFAULT_FW_NAME);
		break;
	case PCIE_DEVICE_ID_MARVELL_88W8897:
		mwifiex_write_reg(adapter, 0x0c58, 0x80c00000);
		mwifiex_read_reg(adapter, 0x0c58, &revision_id);
		revision_id &= 0xff00;
		switch (revision_id) {
		case PCIE8897_A0:
			strcpy(adapter->fw_name, PCIE8897_A0_FW_NAME);
			break;
		case PCIE8897_B0:
			strcpy(adapter->fw_name, PCIE8897_B0_FW_NAME);
			break;
		default:
			strcpy(adapter->fw_name, PCIE8897_DEFAULT_FW_NAME);

			break;
		}
		break;
	case PCIE_DEVICE_ID_MARVELL_88W8997:
		mwifiex_read_reg(adapter, 0x8, &revision_id);
		mwifiex_read_reg(adapter, 0x0cd0, &version);
		mwifiex_read_reg(adapter, 0x0cd4, &magic);
		revision_id &= 0xff;
		version &= 0x7;
		magic &= 0xff;
		if (revision_id == PCIE8997_A1 &&
		    magic == CHIP_MAGIC_VALUE &&
		    version == CHIP_VER_PCIEUART)
			strcpy(adapter->fw_name, PCIEUART8997_FW_NAME_V4);
		else
			strcpy(adapter->fw_name, PCIEUSB8997_FW_NAME_V4);
		break;
	default:
		break;
	}
}