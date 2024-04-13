warn_changed_key(Key *host_key)
{
	char *fp;

	fp = key_fingerprint(host_key, SSH_FP_MD5, SSH_FP_HEX);

	error("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	error("@    WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!     @");
	error("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	error("IT IS POSSIBLE THAT SOMEONE IS DOING SOMETHING NASTY!");
	error("Someone could be eavesdropping on you right now (man-in-the-middle attack)!");
	error("It is also possible that a host key has just been changed.");
	error("The fingerprint for the %s key sent by the remote host is\n%s.",
	    key_type(host_key), fp);
	error("Please contact your system administrator.");

	free(fp);
}