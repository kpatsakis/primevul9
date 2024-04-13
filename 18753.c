static void __attribute__((__noreturn__)) usage(FILE *out)
{
	fputs(USAGE_HEADER, out);
	fprintf(out, _(
		" %1$s [-lhV]\n"
		" %1$s -a [options]\n"
		" %1$s [options] [--source] <source> | [--target] <directory>\n"
		" %1$s [options] <source> <directory>\n"
		" %1$s <operation> <mountpoint> [<target>]\n"),
		program_invocation_short_name);

	fputs(USAGE_OPTIONS, out);
	fprintf(out, _(
	" -a, --all               mount all filesystems mentioned in fstab\n"
	" -c, --no-canonicalize   don't canonicalize paths\n"
	" -f, --fake              dry run; skip the mount(2) syscall\n"
	" -F, --fork              fork off for each device (use with -a)\n"
	" -T, --fstab <path>      alternative file to /etc/fstab\n"));
	fprintf(out, _(
	" -h, --help              display this help text and exit\n"
	" -i, --internal-only     don't call the mount.<type> helpers\n"
	" -l, --show-labels       lists all mounts with LABELs\n"
	" -n, --no-mtab           don't write to /etc/mtab\n"));
	fprintf(out, _(
	" -o, --options <list>    comma-separated list of mount options\n"
	" -O, --test-opts <list>  limit the set of filesystems (use with -a)\n"
	" -r, --read-only         mount the filesystem read-only (same as -o ro)\n"
	" -t, --types <list>      limit the set of filesystem types\n"));
	fprintf(out, _(
	"     --source <src>      explicitly specifies source (path, label, uuid)\n"
	"     --target <target>   explicitly specifies mountpoint\n"));
	fprintf(out, _(
	" -v, --verbose           say what is being done\n"
	" -V, --version           display version information and exit\n"
	" -w, --read-write        mount the filesystem read-write (default)\n"));

	fputs(USAGE_SEPARATOR, out);
	fputs(USAGE_HELP, out);
	fputs(USAGE_VERSION, out);

	fprintf(out, _(
	"\nSource:\n"
	" -L, --label <label>     synonym for LABEL=<label>\n"
	" -U, --uuid <uuid>       synonym for UUID=<uuid>\n"
	" LABEL=<label>           specifies device by filesystem label\n"
	" UUID=<uuid>             specifies device by filesystem UUID\n"
	" PARTLABEL=<label>       specifies device by partition label\n"
	" PARTUUID=<uuid>         specifies device by partition UUID\n"));

	fprintf(out, _(
	" <device>                specifies device by path\n"
	" <directory>             mountpoint for bind mounts (see --bind/rbind)\n"
	" <file>                  regular file for loopdev setup\n"));

	fprintf(out, _(
	"\nOperations:\n"
	" -B, --bind              mount a subtree somewhere else (same as -o bind)\n"
	" -M, --move              move a subtree to some other place\n"
	" -R, --rbind             mount a subtree and all submounts somewhere else\n"));
	fprintf(out, _(
	" --make-shared           mark a subtree as shared\n"
	" --make-slave            mark a subtree as slave\n"
	" --make-private          mark a subtree as private\n"
	" --make-unbindable       mark a subtree as unbindable\n"));
	fprintf(out, _(
	" --make-rshared          recursively mark a whole subtree as shared\n"
	" --make-rslave           recursively mark a whole subtree as slave\n"
	" --make-rprivate         recursively mark a whole subtree as private\n"
	" --make-runbindable      recursively mark a whole subtree as unbindable\n"));

	fprintf(out, USAGE_MAN_TAIL("mount(8)"));

	exit(out == stderr ? MOUNT_EX_USAGE : MOUNT_EX_SUCCESS);
}