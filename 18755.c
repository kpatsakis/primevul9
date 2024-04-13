static void selinux_warning(struct libmnt_context *cxt, const char *tgt)
{

	if (tgt && mnt_context_is_verbose(cxt) && is_selinux_enabled() > 0) {
		security_context_t raw = NULL, def = NULL;

		if (getfilecon(tgt, &raw) > 0
		    && security_get_initial_context("file", &def) == 0) {

		if (!selinux_file_context_cmp(raw, def))
			printf(_(
	"mount: %s does not contain SELinux labels.\n"
	"       You just mounted an file system that supports labels which does not\n"
	"       contain labels, onto an SELinux box. It is likely that confined\n"
	"       applications will generate AVC messages and not be allowed access to\n"
	"       this file system.  For more details see restorecon(8) and mount(8).\n"),
				tgt);
		}
		freecon(raw);
		freecon(def);
	}
}