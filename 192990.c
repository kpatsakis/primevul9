static void snd_seq_info_dump_ports(struct snd_info_buffer *buffer,
				    struct snd_seq_client *client)
{
	struct snd_seq_client_port *p;

	mutex_lock(&client->ports_mutex);
	list_for_each_entry(p, &client->ports_list_head, list) {
		snd_iprintf(buffer, "  Port %3d : \"%s\" (%c%c%c%c)\n",
			    p->addr.port, p->name,
			    FLAG_PERM_RD(p->capability),
			    FLAG_PERM_WR(p->capability),
			    FLAG_PERM_EX(p->capability),
			    FLAG_PERM_DUPLEX(p->capability));
		snd_seq_info_dump_subscribers(buffer, &p->c_src, 1, "    Connecting To: ");
		snd_seq_info_dump_subscribers(buffer, &p->c_dest, 0, "    Connected From: ");
	}
	mutex_unlock(&client->ports_mutex);
}