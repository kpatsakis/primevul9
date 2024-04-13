int splitexport(CLIENT* client) {
	off_t i;
	int fhandle;

	client->export = g_array_new(TRUE, TRUE, sizeof(int));
	for (i=0; i<client->exportsize; i+=client->server->hunksize) {
		gchar *tmpname;

		if(client->server->flags & F_MULTIFILE) {
			tmpname=g_strdup_printf("%s.%d", client->exportname,
					(int)(i/client->server->hunksize));
		} else {
			tmpname=g_strdup(client->exportname);
		}
		DEBUG2( "Opening %s\n", tmpname );
		if((fhandle = open(tmpname, (client->server->flags & F_READONLY) ? O_RDONLY : O_RDWR)) == -1) {
			/* Read WRITE ACCESS was requested by media is only read only */
			client->server->flags |= F_AUTOREADONLY;
			client->server->flags |= F_READONLY;
			if((fhandle = open(tmpname, O_RDONLY)) == -1)
				err("Could not open exported file: %m");
		}
		g_array_insert_val(client->export,i/client->server->hunksize,fhandle);
		g_free(tmpname);
	}
	return 0;
}