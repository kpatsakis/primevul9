static irqreturn_t fsl_hv_state_change_isr(int irq, void *data)
{
	unsigned int status;
	struct doorbell_isr *dbisr = data;
	int ret;

	/* It's still a doorbell, so add it to all the queues. */
	fsl_hv_queue_doorbell(dbisr->doorbell);

	/* Determine the new state, and if it's stopped, notify the clients. */
	ret = fh_partition_get_status(dbisr->partition, &status);
	if (!ret && (status == FH_PARTITION_STOPPED))
		return IRQ_WAKE_THREAD;

	return IRQ_HANDLED;
}