ssize_t sdma_set_cpu_to_sde_map(struct sdma_engine *sde, const char *buf,
				size_t count)
{
	struct hfi1_devdata *dd = sde->dd;
	cpumask_var_t mask, new_mask;
	unsigned long cpu;
	int ret, vl, sz;
	struct sdma_rht_node *rht_node;

	vl = sdma_engine_get_vl(sde);
	if (unlikely(vl < 0 || vl >= ARRAY_SIZE(rht_node->map)))
		return -EINVAL;

	ret = zalloc_cpumask_var(&mask, GFP_KERNEL);
	if (!ret)
		return -ENOMEM;

	ret = zalloc_cpumask_var(&new_mask, GFP_KERNEL);
	if (!ret) {
		free_cpumask_var(mask);
		return -ENOMEM;
	}
	ret = cpulist_parse(buf, mask);
	if (ret)
		goto out_free;

	if (!cpumask_subset(mask, cpu_online_mask)) {
		dd_dev_warn(sde->dd, "Invalid CPU mask\n");
		ret = -EINVAL;
		goto out_free;
	}

	sz = sizeof(struct sdma_rht_map_elem) +
			(TXE_NUM_SDMA_ENGINES * sizeof(struct sdma_engine *));

	mutex_lock(&process_to_sde_mutex);

	for_each_cpu(cpu, mask) {
		/* Check if we have this already mapped */
		if (cpumask_test_cpu(cpu, &sde->cpu_mask)) {
			cpumask_set_cpu(cpu, new_mask);
			continue;
		}

		rht_node = rhashtable_lookup_fast(dd->sdma_rht, &cpu,
						  sdma_rht_params);
		if (!rht_node) {
			rht_node = kzalloc(sizeof(*rht_node), GFP_KERNEL);
			if (!rht_node) {
				ret = -ENOMEM;
				goto out;
			}

			rht_node->map[vl] = kzalloc(sz, GFP_KERNEL);
			if (!rht_node->map[vl]) {
				kfree(rht_node);
				ret = -ENOMEM;
				goto out;
			}
			rht_node->cpu_id = cpu;
			rht_node->map[vl]->mask = 0;
			rht_node->map[vl]->ctr = 1;
			rht_node->map[vl]->sde[0] = sde;

			ret = rhashtable_insert_fast(dd->sdma_rht,
						     &rht_node->node,
						     sdma_rht_params);
			if (ret) {
				kfree(rht_node->map[vl]);
				kfree(rht_node);
				dd_dev_err(sde->dd, "Failed to set process to sde affinity for cpu %lu\n",
					   cpu);
				goto out;
			}

		} else {
			int ctr, pow;

			/* Add new user mappings */
			if (!rht_node->map[vl])
				rht_node->map[vl] = kzalloc(sz, GFP_KERNEL);

			if (!rht_node->map[vl]) {
				ret = -ENOMEM;
				goto out;
			}

			rht_node->map[vl]->ctr++;
			ctr = rht_node->map[vl]->ctr;
			rht_node->map[vl]->sde[ctr - 1] = sde;
			pow = roundup_pow_of_two(ctr);
			rht_node->map[vl]->mask = pow - 1;

			/* Populate the sde map table */
			sdma_populate_sde_map(rht_node->map[vl]);
		}
		cpumask_set_cpu(cpu, new_mask);
	}

	/* Clean up old mappings */
	for_each_cpu(cpu, cpu_online_mask) {
		struct sdma_rht_node *rht_node;

		/* Don't cleanup sdes that are set in the new mask */
		if (cpumask_test_cpu(cpu, mask))
			continue;

		rht_node = rhashtable_lookup_fast(dd->sdma_rht, &cpu,
						  sdma_rht_params);
		if (rht_node) {
			bool empty = true;
			int i;

			/* Remove mappings for old sde */
			for (i = 0; i < HFI1_MAX_VLS_SUPPORTED; i++)
				if (rht_node->map[i])
					sdma_cleanup_sde_map(rht_node->map[i],
							     sde);

			/* Free empty hash table entries */
			for (i = 0; i < HFI1_MAX_VLS_SUPPORTED; i++) {
				if (!rht_node->map[i])
					continue;

				if (rht_node->map[i]->ctr) {
					empty = false;
					break;
				}
			}

			if (empty) {
				ret = rhashtable_remove_fast(dd->sdma_rht,
							     &rht_node->node,
							     sdma_rht_params);
				WARN_ON(ret);

				for (i = 0; i < HFI1_MAX_VLS_SUPPORTED; i++)
					kfree(rht_node->map[i]);

				kfree(rht_node);
			}
		}
	}

	cpumask_copy(&sde->cpu_mask, new_mask);
out:
	mutex_unlock(&process_to_sde_mutex);
out_free:
	free_cpumask_var(mask);
	free_cpumask_var(new_mask);
	return ret ? : strnlen(buf, PAGE_SIZE);
}