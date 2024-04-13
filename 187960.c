static int __init fsl_hypervisor_init(void)
{
	struct device_node *np;
	struct doorbell_isr *dbisr, *n;
	int ret;

	pr_info("Freescale hypervisor management driver\n");

	if (!has_fsl_hypervisor()) {
		pr_info("fsl-hv: no hypervisor found\n");
		return -ENODEV;
	}

	ret = misc_register(&fsl_hv_misc_dev);
	if (ret) {
		pr_err("fsl-hv: cannot register device\n");
		return ret;
	}

	INIT_LIST_HEAD(&db_list);
	INIT_LIST_HEAD(&isr_list);

	for_each_compatible_node(np, NULL, "epapr,hv-receive-doorbell") {
		unsigned int irq;
		const uint32_t *handle;

		handle = of_get_property(np, "interrupts", NULL);
		irq = irq_of_parse_and_map(np, 0);
		if (!handle || (irq == NO_IRQ)) {
			pr_err("fsl-hv: no 'interrupts' property in %pOF node\n",
				np);
			continue;
		}

		dbisr = kzalloc(sizeof(*dbisr), GFP_KERNEL);
		if (!dbisr)
			goto out_of_memory;

		dbisr->irq = irq;
		dbisr->doorbell = be32_to_cpup(handle);

		if (of_device_is_compatible(np, "fsl,hv-shutdown-doorbell")) {
			/* The shutdown doorbell gets its own ISR */
			ret = request_irq(irq, fsl_hv_shutdown_isr, 0,
					  np->name, NULL);
		} else if (of_device_is_compatible(np,
			"fsl,hv-state-change-doorbell")) {
			/*
			 * The state change doorbell triggers a notification if
			 * the state of the managed partition changes to
			 * "stopped". We need a separate interrupt handler for
			 * that, and we also need to know the handle of the
			 * target partition, not just the handle of the
			 * doorbell.
			 */
			dbisr->partition = ret = get_parent_handle(np);
			if (ret < 0) {
				pr_err("fsl-hv: node %pOF has missing or "
				       "malformed parent\n", np);
				kfree(dbisr);
				continue;
			}
			ret = request_threaded_irq(irq, fsl_hv_state_change_isr,
						   fsl_hv_state_change_thread,
						   0, np->name, dbisr);
		} else
			ret = request_irq(irq, fsl_hv_isr, 0, np->name, dbisr);

		if (ret < 0) {
			pr_err("fsl-hv: could not request irq %u for node %pOF\n",
			       irq, np);
			kfree(dbisr);
			continue;
		}

		list_add(&dbisr->list, &isr_list);

		pr_info("fsl-hv: registered handler for doorbell %u\n",
			dbisr->doorbell);
	}

	return 0;

out_of_memory:
	list_for_each_entry_safe(dbisr, n, &isr_list, list) {
		free_irq(dbisr->irq, dbisr);
		list_del(&dbisr->list);
		kfree(dbisr);
	}

	misc_deregister(&fsl_hv_misc_dev);

	return -ENOMEM;
}