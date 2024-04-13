static int net_client_init(QemuOpts *opts, bool is_netdev, Error **errp)
{
    gchar **substrings = NULL;
    Netdev *object = NULL;
    int ret = -1;
    Visitor *v = opts_visitor_new(opts);

    /* Parse convenience option format ip6-net=fec0::0[/64] */
    const char *ip6_net = qemu_opt_get(opts, "ipv6-net");

    if (ip6_net) {
        char *prefix_addr;
        unsigned long prefix_len = 64; /* Default 64bit prefix length. */

        substrings = g_strsplit(ip6_net, "/", 2);
        if (!substrings || !substrings[0]) {
            error_setg(errp, QERR_INVALID_PARAMETER_VALUE, "ipv6-net",
                       "a valid IPv6 prefix");
            goto out;
        }

        prefix_addr = substrings[0];

        /* Handle user-specified prefix length. */
        if (substrings[1] &&
            qemu_strtoul(substrings[1], NULL, 10, &prefix_len))
        {
            error_setg(errp, QERR_INVALID_PARAMETER_VALUE,
                       "ipv6-prefixlen", "a number");
            goto out;
        }

        qemu_opt_set(opts, "ipv6-prefix", prefix_addr, &error_abort);
        qemu_opt_set_number(opts, "ipv6-prefixlen", prefix_len,
                            &error_abort);
        qemu_opt_unset(opts, "ipv6-net");
    }

    /* Create an ID for -net if the user did not specify one */
    if (!is_netdev && !qemu_opts_id(opts)) {
        qemu_opts_set_id(opts, id_generate(ID_NET));
    }

    if (visit_type_Netdev(v, NULL, &object, errp)) {
        ret = net_client_init1(object, is_netdev, errp);
    }

    qapi_free_Netdev(object);

out:
    g_strfreev(substrings);
    visit_free(v);
    return ret;
}