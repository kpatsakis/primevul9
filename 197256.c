void qmp_netdev_add(QDict *qdict, QObject **ret, Error **errp)
{
    Error *local_err = NULL;
    QemuOptsList *opts_list;
    QemuOpts *opts;

    opts_list = qemu_find_opts_err("netdev", &local_err);
    if (local_err) {
        goto out;
    }

    opts = qemu_opts_from_qdict(opts_list, qdict, &local_err);
    if (local_err) {
        goto out;
    }

    netdev_add(opts, &local_err);
    if (local_err) {
        qemu_opts_del(opts);
        goto out;
    }

out:
    error_propagate(errp, local_err);
}