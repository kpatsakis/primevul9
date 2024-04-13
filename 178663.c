static void __attribute__((__noreturn__)) usage(FILE *out)
{
	fputs(USAGE_HEADER, out);
	fprintf(out, _(
		" %1$s [-hV]\n"
		" %1$s -a [options]\n"
		" %1$s [options] <source> | <directory>\n"),
		program_invocation_short_name);

	fputs(USAGE_OPTIONS, out);
	fprintf(out, _(
	" -a, --all               umount all filesystems\n"
	" -c, --no-canonicalize   don't canonicalize paths\n"
	" -d, --detach-loop       if mounted loop device, also free this loop device\n"
	"     --fake              dry run; skip the umount(2) syscall\n"
	" -f, --force             force unmount (in case of an unreachable NFS system)\n"));
	fprintf(out, _(
	" -i, --internal-only     don't call the umount.<type> helpers\n"
	" -n, --no-mtab           don't write to /etc/mtab\n"
	" -l, --lazy              detach the filesystem now, and cleanup all later\n"));
	fprintf(out, _(
	" -O, --test-opts <list>  limit the set of filesystems (use with -a)\n"
	" -R, --recursive         recursively unmount a target with all its children\n"
	" -r, --read-only         In case unmounting fails, try to remount read-only\n"
	" -t, --types <list>      limit the set of filesystem types\n"
	" -v, --verbose           say what is being done\n"));

	fputs(USAGE_SEPARATOR, out);
	fputs(USAGE_HELP, out);
	fputs(USAGE_VERSION, out);
	fprintf(out, USAGE_MAN_TAIL("umount(8)"));

	exit(out == stderr ? MOUNT_EX_USAGE : MOUNT_EX_SUCCESS);
}