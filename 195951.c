int net_client_parse(QemuOptsList *opts_list, const char *optarg)
{
    if (!qemu_opts_parse_noisily(opts_list, optarg, true)) {
        return -1;
    }

    return 0;
}