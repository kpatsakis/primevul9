static void ql_sem_unlock(struct ql3_adapter *qdev, u32 sem_mask)
{
	struct ql3xxx_port_registers __iomem *port_regs =
		qdev->mem_map_registers;
	writel(sem_mask, &port_regs->CommonRegs.semaphoreReg);
	readl(&port_regs->CommonRegs.semaphoreReg);
}