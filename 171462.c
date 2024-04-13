static int ql_sem_spinlock(struct ql3_adapter *qdev,
			    u32 sem_mask, u32 sem_bits)
{
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	u32 value;
	unsigned int seconds = 3;

	do {
		writel((sem_mask | sem_bits),
		       &port_regs->CommonRegs.semaphoreReg);
		value = readl(&port_regs->CommonRegs.semaphoreReg);
		if ((value & (sem_mask >> 16)) == sem_bits)
			return 0;
		ssleep(1);
	} while (--seconds);
	return -1;
}