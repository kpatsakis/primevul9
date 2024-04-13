nwfilterLookupByUUID(virConnectPtr conn,
                     const unsigned char *uuid)
{
    virNWFilterObj *obj;
    virNWFilterDef *def;
    virNWFilterPtr nwfilter = NULL;

    nwfilterDriverLock();
    obj = nwfilterObjFromNWFilter(uuid);
    nwfilterDriverUnlock();

    if (!obj)
        return NULL;
    def = virNWFilterObjGetDef(obj);

    if (virNWFilterLookupByUUIDEnsureACL(conn, def) < 0)
        goto cleanup;

    nwfilter = virGetNWFilter(conn, def->name, def->uuid);

 cleanup:
    virNWFilterObjUnlock(obj);
    return nwfilter;
}