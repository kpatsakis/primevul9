int rawexpwrite(off_t a, char *buf, size_t len, CLIENT *client) {
	ssize_t res;

	myseek(g_array_index(client->export, int, (int)(a/client->server->hunksize)), a%client->server->hunksize);
	;
	res = write(g_array_index(client->export, int, (int)((off_t)a/(off_t)(client->server->hunksize))), buf, len);
	return (res < 0 || (size_t)res != len);
}