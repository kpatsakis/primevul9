check_host_cert(const char *host, const Key *host_key)
{
	const char *reason;

	if (key_cert_check_authority(host_key, 1, 0, host, &reason) != 0) {
		error("%s", reason);
		return 0;
	}
	if (buffer_len(&host_key->cert->critical) != 0) {
		error("Certificate for %s contains unsupported "
		    "critical options(s)", host);
		return 0;
	}
	return 1;
}