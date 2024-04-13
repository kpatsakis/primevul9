nwfilterUndefine(virNWFilterPtr nwfilter)
{
    virNWFilterObj *obj;
    virNWFilterDef *def;
    int ret = -1;

    nwfilterDriverLock();
    virNWFilterWriteLockFilterUpdates();

    if (!(obj = nwfilterObjFromNWFilter(nwfilter->uuid)))
        goto cleanup;
    def = virNWFilterObjGetDef(obj);

    if (virNWFilterUndefineEnsureACL(nwfilter->conn, def) < 0)
        goto cleanup;

    if (virNWFilterObjTestUnassignDef(obj) < 0) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       "%s",
                       _("nwfilter is in use"));
        goto cleanup;
    }

    if (virNWFilterDeleteDef(driver->configDir, def) < 0)
        goto cleanup;

    virNWFilterObjListRemove(driver->nwfilters, obj);
    obj = NULL;
    ret = 0;

 cleanup:
    if (obj)
        virNWFilterObjUnlock(obj);

    virNWFilterUnlockFilterUpdates();
    nwfilterDriverUnlock();
    return ret;
}