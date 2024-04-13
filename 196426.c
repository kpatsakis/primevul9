nwfilterDefineXMLFlags(virConnectPtr conn,
                       const char *xml,
                       unsigned int flags)
{
    virNWFilterDef *def;
    virNWFilterObj *obj = NULL;
    virNWFilterDef *objdef;
    virNWFilterPtr nwfilter = NULL;

    virCheckFlags(VIR_NWFILTER_DEFINE_VALIDATE, NULL);


    if (!driver->privileged) {
        virReportError(VIR_ERR_OPERATION_INVALID, "%s",
                       _("Can't define NWFilters in session mode"));
        return NULL;
    }

    nwfilterDriverLock();
    virNWFilterWriteLockFilterUpdates();

    if (!(def = virNWFilterDefParseString(xml, flags)))
        goto cleanup;

    if (virNWFilterDefineXMLFlagsEnsureACL(conn, def) < 0)
        goto cleanup;

    if (!(obj = virNWFilterObjListAssignDef(driver->nwfilters, def)))
        goto cleanup;
    def = NULL;
    objdef = virNWFilterObjGetDef(obj);

    if (virNWFilterSaveConfig(driver->configDir, objdef) < 0) {
        virNWFilterObjListRemove(driver->nwfilters, obj);
        goto cleanup;
    }

    nwfilter = virGetNWFilter(conn, objdef->name, objdef->uuid);

 cleanup:
    virNWFilterDefFree(def);
    if (obj)
        virNWFilterObjUnlock(obj);

    virNWFilterUnlockFilterUpdates();
    nwfilterDriverUnlock();
    return nwfilter;
}