add_to_client(struct i915_request *rq, struct drm_file *file)
{
	rq->file_priv = file->driver_priv;
	list_add_tail(&rq->client_link, &rq->file_priv->mm.request_list);
}