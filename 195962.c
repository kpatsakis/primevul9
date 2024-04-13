void show_netdevs(void)
{
    int idx;
    const char *available_netdevs[] = {
        "socket",
        "hubport",
        "tap",
#ifdef CONFIG_SLIRP
        "user",
#endif
#ifdef CONFIG_L2TPV3
        "l2tpv3",
#endif
#ifdef CONFIG_VDE
        "vde",
#endif
#ifdef CONFIG_NET_BRIDGE
        "bridge",
#endif
#ifdef CONFIG_NETMAP
        "netmap",
#endif
#ifdef CONFIG_POSIX
        "vhost-user",
#endif
#ifdef CONFIG_VHOST_VDPA
        "vhost-vdpa",
#endif
    };

    qemu_printf("Available netdev backend types:\n");
    for (idx = 0; idx < ARRAY_SIZE(available_netdevs); idx++) {
        qemu_printf("%s\n", available_netdevs[idx]);
    }
}