nwfilterGetXMLDesc(virNWFilterPtr nwfilter,
                   unsigned int flags)
{
    virNWFilterObj *obj;
    virNWFilterDef *def;
    char *ret = NULL;

    virCheckFlags(0, NULL);

    nwfilterDriverLock();
    obj = nwfilterObjFromNWFilter(nwfilter->uuid);
    nwfilterDriverUnlock();

    if (!obj)
        return NULL;
    def = virNWFilterObjGetDef(obj);

    if (virNWFilterGetXMLDescEnsureACL(nwfilter->conn, def) < 0)
        goto cleanup;

    ret = virNWFilterDefFormat(def);

 cleanup:
    virNWFilterObjUnlock(obj);
    return ret;
}