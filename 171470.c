static enum PHY_DEVICE_TYPE getPhyType(struct ql3_adapter *qdev,
				       u16 phyIdReg0, u16 phyIdReg1)
{
	enum PHY_DEVICE_TYPE result = PHY_TYPE_UNKNOWN;
	u32   oui;
	u16   model;
	int i;

	if (phyIdReg0 == 0xffff)
		return result;

	if (phyIdReg1 == 0xffff)
		return result;

	/* oui is split between two registers */
	oui = (phyIdReg0 << 6) | ((phyIdReg1 & PHY_OUI_1_MASK) >> 10);

	model = (phyIdReg1 & PHY_MODEL_MASK) >> 4;

	/* Scan table for this PHY */
	for (i = 0; i < MAX_PHY_DEV_TYPES; i++) {
		if ((oui == PHY_DEVICES[i].phyIdOUI) &&
		    (model == PHY_DEVICES[i].phyIdModel)) {
			netdev_info(qdev->ndev, "Phy: %s\n",
				    PHY_DEVICES[i].name);
			result = PHY_DEVICES[i].phyDevice;
			break;
		}
	}

	return result;
}