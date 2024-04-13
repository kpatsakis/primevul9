static void msg_release_ip(struct messaging_context *msg_ctx, void *private_data,
			   uint32_t msg_type, struct server_id server_id, DATA_BLOB *data)
{
	release_ip((char *)data->data, NULL);
}