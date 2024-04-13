
static int ql_init_misc_registers(struct ql3_adapter *qdev)
{
	struct ql3xxx_local_ram_registers __iomem *local_ram =
	    (void __iomem *)qdev->mem_map_registers;

	if (ql_sem_spinlock(qdev, QL_DDR_RAM_SEM_MASK,
			(QL_RESOURCE_BITS_BASE_CODE | (qdev->mac_index) *
			 2) << 4))
		return -1;

	ql_write_page2_reg(qdev,
			   &local_ram->bufletSize, qdev->nvram_data.bufletSize);

	ql_write_page2_reg(qdev,
			   &local_ram->maxBufletCount,
			   qdev->nvram_data.bufletCount);

	ql_write_page2_reg(qdev,
			   &local_ram->freeBufletThresholdLow,
			   (qdev->nvram_data.tcpWindowThreshold25 << 16) |
			   (qdev->nvram_data.tcpWindowThreshold0));

	ql_write_page2_reg(qdev,
			   &local_ram->freeBufletThresholdHigh,
			   qdev->nvram_data.tcpWindowThreshold50);

	ql_write_page2_reg(qdev,
			   &local_ram->ipHashTableBase,
			   (qdev->nvram_data.ipHashTableBaseHi << 16) |
			   qdev->nvram_data.ipHashTableBaseLo);
	ql_write_page2_reg(qdev,
			   &local_ram->ipHashTableCount,
			   qdev->nvram_data.ipHashTableSize);
	ql_write_page2_reg(qdev,
			   &local_ram->tcpHashTableBase,
			   (qdev->nvram_data.tcpHashTableBaseHi << 16) |
			   qdev->nvram_data.tcpHashTableBaseLo);
	ql_write_page2_reg(qdev,
			   &local_ram->tcpHashTableCount,
			   qdev->nvram_data.tcpHashTableSize);
	ql_write_page2_reg(qdev,
			   &local_ram->ncbBase,
			   (qdev->nvram_data.ncbTableBaseHi << 16) |
			   qdev->nvram_data.ncbTableBaseLo);
	ql_write_page2_reg(qdev,
			   &local_ram->maxNcbCount,
			   qdev->nvram_data.ncbTableSize);
	ql_write_page2_reg(qdev,
			   &local_ram->drbBase,
			   (qdev->nvram_data.drbTableBaseHi << 16) |
			   qdev->nvram_data.drbTableBaseLo);
	ql_write_page2_reg(qdev,
			   &local_ram->maxDrbCount,
			   qdev->nvram_data.drbTableSize);
	ql_sem_unlock(qdev, QL_DDR_RAM_SEM_MASK);
	return 0;