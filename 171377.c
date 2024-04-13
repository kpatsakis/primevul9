static int ql_get_nvram_params(struct ql3_adapter *qdev)
{
	u16 *pEEPROMData;
	u16 checksum = 0;
	u32 index;
	unsigned long hw_flags;

	spin_lock_irqsave(&qdev->hw_lock, hw_flags);

	pEEPROMData = (u16 *)&qdev->nvram_data;
	qdev->eeprom_cmd_data = 0;
	if (ql_sem_spinlock(qdev, QL_NVRAM_SEM_MASK,
			(QL_RESOURCE_BITS_BASE_CODE | (qdev->mac_index) *
			 2) << 10)) {
		pr_err("%s: Failed ql_sem_spinlock()\n", __func__);
		spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);
		return -1;
	}

	for (index = 0; index < EEPROM_SIZE; index++) {
		eeprom_readword(qdev, index, pEEPROMData);
		checksum += *pEEPROMData;
		pEEPROMData++;
	}
	ql_sem_unlock(qdev, QL_NVRAM_SEM_MASK);

	if (checksum != 0) {
		netdev_err(qdev->ndev, "checksum should be zero, is %x!!\n",
			   checksum);
		spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);
		return -1;
	}

	spin_unlock_irqrestore(&qdev->hw_lock, hw_flags);
	return checksum;
}