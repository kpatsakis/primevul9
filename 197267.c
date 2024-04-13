static int net_client_init(QemuOpts *opts, bool is_netdev, Error **errp)
{
    void *object = NULL;
    Error *err = NULL;
    int ret = -1;
    Visitor *v = opts_visitor_new(opts);

    const char *type = qemu_opt_get(opts, "type");

    if (is_netdev && type && is_help_option(type)) {
        show_netdevs();
        exit(0);
    } else {
        /* Parse convenience option format ip6-net=fec0::0[/64] */
        const char *ip6_net = qemu_opt_get(opts, "ipv6-net");

        if (ip6_net) {
            char buf[strlen(ip6_net) + 1];

            if (get_str_sep(buf, sizeof(buf), &ip6_net, '/') < 0) {
                /* Default 64bit prefix length.  */
                qemu_opt_set(opts, "ipv6-prefix", ip6_net, &error_abort);
                qemu_opt_set_number(opts, "ipv6-prefixlen", 64, &error_abort);
            } else {
                /* User-specified prefix length.  */
                unsigned long len;
                int err;

                qemu_opt_set(opts, "ipv6-prefix", buf, &error_abort);
                err = qemu_strtoul(ip6_net, NULL, 10, &len);

                if (err) {
                    error_setg(errp, QERR_INVALID_PARAMETER_VALUE,
                              "ipv6-prefix", "a number");
                } else {
                    qemu_opt_set_number(opts, "ipv6-prefixlen", len,
                                        &error_abort);
                }
            }
            qemu_opt_unset(opts, "ipv6-net");
        }
    }

    if (is_netdev) {
        visit_type_Netdev(v, NULL, (Netdev **)&object, &err);
    } else {
        visit_type_NetLegacy(v, NULL, (NetLegacy **)&object, &err);
    }

    if (!err) {
        ret = net_client_init1(object, is_netdev, &err);
    }

    if (is_netdev) {
        qapi_free_Netdev(object);
    } else {
        qapi_free_NetLegacy(object);
    }

    error_propagate(errp, err);
    visit_free(v);
    return ret;
}