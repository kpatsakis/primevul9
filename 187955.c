static irqreturn_t fsl_hv_shutdown_isr(int irq, void *data)
{
	orderly_poweroff(false);

	return IRQ_HANDLED;
}