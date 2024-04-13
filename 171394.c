static int ql_sem_lock(struct ql3_adapter *qdev, u32 sem_mask, u32 sem_bits)
{
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	u32 value;

	writel((sem_mask | sem_bits), &port_regs->CommonRegs.semaphoreReg);
	value = readl(&port_regs->CommonRegs.semaphoreReg);
	return ((value & (sem_mask >> 16)) == sem_bits);
}