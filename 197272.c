static int net_param_nic(void *dummy, QemuOpts *opts, Error **errp)
{
    char *mac, *nd_id;
    int idx, ret;
    NICInfo *ni;
    const char *type;

    type = qemu_opt_get(opts, "type");
    if (type && g_str_equal(type, "none")) {
        return 0;    /* Nothing to do, default_net is cleared in vl.c */
    }

    idx = nic_get_free_idx();
    if (idx == -1 || nb_nics >= MAX_NICS) {
        error_setg(errp, "no more on-board/default NIC slots available");
        return -1;
    }

    if (!type) {
        qemu_opt_set(opts, "type", "user", &error_abort);
    }

    ni = &nd_table[idx];
    memset(ni, 0, sizeof(*ni));
    ni->model = qemu_opt_get_del(opts, "model");

    /* Create an ID if the user did not specify one */
    nd_id = g_strdup(qemu_opts_id(opts));
    if (!nd_id) {
        nd_id = g_strdup_printf("__org.qemu.nic%i\n", idx);
        qemu_opts_set_id(opts, nd_id);
    }

    /* Handle MAC address */
    mac = qemu_opt_get_del(opts, "mac");
    if (mac) {
        ret = net_parse_macaddr(ni->macaddr.a, mac);
        g_free(mac);
        if (ret) {
            error_setg(errp, "invalid syntax for ethernet address");
            goto out;
        }
        if (is_multicast_ether_addr(ni->macaddr.a)) {
            error_setg(errp, "NIC cannot have multicast MAC address");
            ret = -1;
            goto out;
        }
    }
    qemu_macaddr_default_if_unset(&ni->macaddr);

    ret = net_client_init(opts, true, errp);
    if (ret == 0) {
        ni->netdev = qemu_find_netdev(nd_id);
        ni->used = true;
        nb_nics++;
    }

out:
    g_free(nd_id);
    return ret;
}