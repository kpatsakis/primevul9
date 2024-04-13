void mdesc_register_notifier(struct mdesc_notifier_client *client)
{
	bool supported = false;
	u64 node;
	int i;

	mutex_lock(&mdesc_mutex);

	/* check to see if the node is supported for registration */
	for (i = 0; md_node_ops_table[i].name != NULL; i++) {
		if (strcmp(md_node_ops_table[i].name, client->node_name) == 0) {
			supported = true;
			break;
		}
	}

	if (!supported) {
		pr_err("MD: %s node not supported\n", client->node_name);
		mutex_unlock(&mdesc_mutex);
		return;
	}

	client->next = client_list;
	client_list = client;

	mdesc_for_each_node_by_name(cur_mdesc, node, client->node_name)
		client->add(cur_mdesc, node, client->node_name);

	mutex_unlock(&mdesc_mutex);
}