static irqreturn_t fsl_hv_isr(int irq, void *data)
{
	fsl_hv_queue_doorbell((uintptr_t) data);

	return IRQ_HANDLED;
}