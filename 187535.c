static int sas_ex_bfs_disc(struct asd_sas_port *port)
{
	int res;
	int level;

	do {
		level = port->disc.max_level;
		res = sas_ex_level_discovery(port, level);
		mb();
	} while (level < port->disc.max_level);

	return res;
}