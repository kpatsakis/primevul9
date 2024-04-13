input_free(struct input_ctx *ictx)
{
	u_int	i;

	for (i = 0; i < ictx->param_list_len; i++) {
		if (ictx->param_list[i].type == INPUT_STRING)
			free(ictx->param_list[i].str);
	}

	event_del(&ictx->timer);

	free(ictx->input_buf);
	evbuffer_free(ictx->since_ground);

	free(ictx);
}