static int PHY_Setup(struct ql3_adapter *qdev)
{
	u16   reg1;
	u16   reg2;
	bool  agereAddrChangeNeeded = false;
	u32 miiAddr = 0;
	int err;

	/*  Determine the PHY we are using by reading the ID's */
	err = ql_mii_read_reg(qdev, PHY_ID_0_REG, &reg1);
	if (err != 0) {
		netdev_err(qdev->ndev, "Could not read from reg PHY_ID_0_REG\n");
		return err;
	}

	err = ql_mii_read_reg(qdev, PHY_ID_1_REG, &reg2);
	if (err != 0) {
		netdev_err(qdev->ndev, "Could not read from reg PHY_ID_1_REG\n");
		return err;
	}

	/*  Check if we have a Agere PHY */
	if ((reg1 == 0xffff) || (reg2 == 0xffff)) {

		/* Determine which MII address we should be using
		   determined by the index of the card */
		if (qdev->mac_index == 0)
			miiAddr = MII_AGERE_ADDR_1;
		else
			miiAddr = MII_AGERE_ADDR_2;

		err = ql_mii_read_reg_ex(qdev, PHY_ID_0_REG, &reg1, miiAddr);
		if (err != 0) {
			netdev_err(qdev->ndev,
				   "Could not read from reg PHY_ID_0_REG after Agere detected\n");
			return err;
		}

		err = ql_mii_read_reg_ex(qdev, PHY_ID_1_REG, &reg2, miiAddr);
		if (err != 0) {
			netdev_err(qdev->ndev, "Could not read from reg PHY_ID_1_REG after Agere detected\n");
			return err;
		}

		/*  We need to remember to initialize the Agere PHY */
		agereAddrChangeNeeded = true;
	}

	/*  Determine the particular PHY we have on board to apply
	    PHY specific initializations */
	qdev->phyType = getPhyType(qdev, reg1, reg2);

	if ((qdev->phyType == PHY_AGERE_ET1011C) && agereAddrChangeNeeded) {
		/* need this here so address gets changed */
		phyAgereSpecificInit(qdev, miiAddr);
	} else if (qdev->phyType == PHY_TYPE_UNKNOWN) {
		netdev_err(qdev->ndev, "PHY is unknown\n");
		return -EIO;
	}

	return 0;
}