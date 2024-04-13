nwfilterLookupByName(virConnectPtr conn,
                     const char *name)
{
    virNWFilterObj *obj;
    virNWFilterDef *def;
    virNWFilterPtr nwfilter = NULL;

    nwfilterDriverLock();
    obj = virNWFilterObjListFindByName(driver->nwfilters, name);
    nwfilterDriverUnlock();

    if (!obj) {
        virReportError(VIR_ERR_NO_NWFILTER,
                       _("no nwfilter with matching name '%s'"), name);
        return NULL;
    }
    def = virNWFilterObjGetDef(obj);

    if (virNWFilterLookupByNameEnsureACL(conn, def) < 0)
        goto cleanup;

    nwfilter = virGetNWFilter(conn, def->name, def->uuid);

 cleanup:
    virNWFilterObjUnlock(obj);
    return nwfilter;
}