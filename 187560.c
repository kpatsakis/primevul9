static int smp_execute_task(struct domain_device *dev, void *req, int req_size,
			    void *resp, int resp_size)
{
	struct scatterlist req_sg;
	struct scatterlist resp_sg;

	sg_init_one(&req_sg, req, req_size);
	sg_init_one(&resp_sg, resp, resp_size);
	return smp_execute_task_sg(dev, &req_sg, &resp_sg);
}