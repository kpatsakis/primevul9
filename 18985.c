sodium_misuse_handler(void)
{
	fatal_exit(
		"dnscrypt: libsodium could not be initialized, this typically"
		" happens when no good source of entropy is found. If you run"
		" unbound in a chroot, make sure /dev/urandom is available. See"
		" https://www.unbound.net/documentation/unbound.conf.html");
}