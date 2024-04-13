void ipc_msg_free_info(struct msg_data *msgds)
{
	while (msgds) {
		struct msg_data *next = msgds->next;
		free(msgds);
		msgds = next;
	}
}