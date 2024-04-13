int v9fs_device_realize_common(V9fsState *s, const V9fsTransport *t,
                               Error **errp)
{
    ERRP_GUARD();
    int i, len;
    struct stat stat;
    FsDriverEntry *fse;
    V9fsPath path;
    int rc = 1;

    assert(!s->transport);
    s->transport = t;

    /* initialize pdu allocator */
    QLIST_INIT(&s->free_list);
    QLIST_INIT(&s->active_list);
    for (i = 0; i < MAX_REQ; i++) {
        QLIST_INSERT_HEAD(&s->free_list, &s->pdus[i], next);
        s->pdus[i].s = s;
        s->pdus[i].idx = i;
    }

    v9fs_path_init(&path);

    fse = get_fsdev_fsentry(s->fsconf.fsdev_id);

    if (!fse) {
        /* We don't have a fsdev identified by fsdev_id */
        error_setg(errp, "9pfs device couldn't find fsdev with the "
                   "id = %s",
                   s->fsconf.fsdev_id ? s->fsconf.fsdev_id : "NULL");
        goto out;
    }

    if (!s->fsconf.tag) {
        /* we haven't specified a mount_tag */
        error_setg(errp, "fsdev with id %s needs mount_tag arguments",
                   s->fsconf.fsdev_id);
        goto out;
    }

    s->ctx.export_flags = fse->export_flags;
    s->ctx.fs_root = g_strdup(fse->path);
    s->ctx.exops.get_st_gen = NULL;
    len = strlen(s->fsconf.tag);
    if (len > MAX_TAG_LEN - 1) {
        error_setg(errp, "mount tag '%s' (%d bytes) is longer than "
                   "maximum (%d bytes)", s->fsconf.tag, len, MAX_TAG_LEN - 1);
        goto out;
    }

    s->tag = g_strdup(s->fsconf.tag);
    s->ctx.uid = -1;

    s->ops = fse->ops;

    s->ctx.fmode = fse->fmode;
    s->ctx.dmode = fse->dmode;

    s->fid_list = NULL;
    qemu_co_rwlock_init(&s->rename_lock);

    if (s->ops->init(&s->ctx, errp) < 0) {
        error_prepend(errp, "cannot initialize fsdev '%s': ",
                      s->fsconf.fsdev_id);
        goto out;
    }

    /*
     * Check details of export path, We need to use fs driver
     * call back to do that. Since we are in the init path, we don't
     * use co-routines here.
     */
    if (s->ops->name_to_path(&s->ctx, NULL, "/", &path) < 0) {
        error_setg(errp,
                   "error in converting name to path %s", strerror(errno));
        goto out;
    }
    if (s->ops->lstat(&s->ctx, &path, &stat)) {
        error_setg(errp, "share path %s does not exist", fse->path);
        goto out;
    } else if (!S_ISDIR(stat.st_mode)) {
        error_setg(errp, "share path %s is not a directory", fse->path);
        goto out;
    }

    s->dev_id = stat.st_dev;

    /* init inode remapping : */
    /* hash table for variable length inode suffixes */
    qpd_table_init(&s->qpd_table);
    /* hash table for slow/full inode remapping (most users won't need it) */
    qpf_table_init(&s->qpf_table);
    /* hash table for quick inode remapping */
    qpp_table_init(&s->qpp_table);
    s->qp_ndevices = 0;
    s->qp_affix_next = 1; /* reserve 0 to detect overflow */
    s->qp_fullpath_next = 1;

    s->ctx.fst = &fse->fst;
    fsdev_throttle_init(s->ctx.fst);

    rc = 0;
out:
    if (rc) {
        v9fs_device_unrealize_common(s);
    }
    v9fs_path_free(&path);
    return rc;
}