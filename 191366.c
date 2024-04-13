send_client_banner(int connection_out, int minor1)
{
	/* Send our own protocol version identification. */
	if (compat20) {
		xasprintf(&client_version_string, "SSH-%d.%d-%.100s\r\n",
		    PROTOCOL_MAJOR_2, PROTOCOL_MINOR_2, SSH_VERSION);
	} else {
		xasprintf(&client_version_string, "SSH-%d.%d-%.100s\n",
		    PROTOCOL_MAJOR_1, minor1, SSH_VERSION);
	}
	if (roaming_atomicio(vwrite, connection_out, client_version_string,
	    strlen(client_version_string)) != strlen(client_version_string))
		fatal("write: %.100s", strerror(errno));
	chop(client_version_string);
	debug("Local version string %.100s", client_version_string);
}