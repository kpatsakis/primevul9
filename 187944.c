static irqreturn_t fsl_hv_state_change_thread(int irq, void *data)
{
	struct doorbell_isr *dbisr = data;

	blocking_notifier_call_chain(&failover_subscribers, dbisr->partition,
				     NULL);

	return IRQ_HANDLED;
}