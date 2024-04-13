static int smp_execute_task_sg(struct domain_device *dev,
		struct scatterlist *req, struct scatterlist *resp)
{
	int res, retry;
	struct sas_task *task = NULL;
	struct sas_internal *i =
		to_sas_internal(dev->port->ha->core.shost->transportt);

	mutex_lock(&dev->ex_dev.cmd_mutex);
	for (retry = 0; retry < 3; retry++) {
		if (test_bit(SAS_DEV_GONE, &dev->state)) {
			res = -ECOMM;
			break;
		}

		task = sas_alloc_slow_task(GFP_KERNEL);
		if (!task) {
			res = -ENOMEM;
			break;
		}
		task->dev = dev;
		task->task_proto = dev->tproto;
		task->smp_task.smp_req = *req;
		task->smp_task.smp_resp = *resp;

		task->task_done = smp_task_done;

		task->slow_task->timer.function = smp_task_timedout;
		task->slow_task->timer.expires = jiffies + SMP_TIMEOUT*HZ;
		add_timer(&task->slow_task->timer);

		res = i->dft->lldd_execute_task(task, GFP_KERNEL);

		if (res) {
			del_timer(&task->slow_task->timer);
			pr_notice("executing SMP task failed:%d\n", res);
			break;
		}

		wait_for_completion(&task->slow_task->completion);
		res = -ECOMM;
		if ((task->task_state_flags & SAS_TASK_STATE_ABORTED)) {
			pr_notice("smp task timed out or aborted\n");
			i->dft->lldd_abort_task(task);
			if (!(task->task_state_flags & SAS_TASK_STATE_DONE)) {
				pr_notice("SMP task aborted and not done\n");
				break;
			}
		}
		if (task->task_status.resp == SAS_TASK_COMPLETE &&
		    task->task_status.stat == SAM_STAT_GOOD) {
			res = 0;
			break;
		}
		if (task->task_status.resp == SAS_TASK_COMPLETE &&
		    task->task_status.stat == SAS_DATA_UNDERRUN) {
			/* no error, but return the number of bytes of
			 * underrun */
			res = task->task_status.residual;
			break;
		}
		if (task->task_status.resp == SAS_TASK_COMPLETE &&
		    task->task_status.stat == SAS_DATA_OVERRUN) {
			res = -EMSGSIZE;
			break;
		}
		if (task->task_status.resp == SAS_TASK_UNDELIVERED &&
		    task->task_status.stat == SAS_DEVICE_UNKNOWN)
			break;
		else {
			pr_notice("%s: task to dev %016llx response: 0x%x status 0x%x\n",
				  __func__,
				  SAS_ADDR(dev->sas_addr),
				  task->task_status.resp,
				  task->task_status.stat);
			sas_free_task(task);
			task = NULL;
		}
	}
	mutex_unlock(&dev->ex_dev.cmd_mutex);

	BUG_ON(retry == 3 && task != NULL);
	sas_free_task(task);
	return res;
}