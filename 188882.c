static void i40e_irq_affinity_notify(struct irq_affinity_notify *notify,
				     const cpumask_t *mask)
{
	struct i40e_q_vector *q_vector =
		container_of(notify, struct i40e_q_vector, affinity_notify);

	cpumask_copy(&q_vector->affinity_mask, mask);
}