int rawexpread(off_t a, char *buf, size_t len, CLIENT *client) {
	ssize_t res;

	myseek(g_array_index(client->export,int,(int)a/client->server->hunksize),
			a%client->server->hunksize);
	res = read(g_array_index(client->export,int,(int)a/client->server->hunksize), buf, len);
	return (res < 0 || (size_t)res != len);
}