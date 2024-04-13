usage_excl(void)
{
    debug_decl(usage_excl, SUDO_DEBUG_ARGS);

    sudo_warnx("%s",
	U_("Only one of the -e, -h, -i, -K, -l, -s, -v or -V options may be specified"));
    usage();
}