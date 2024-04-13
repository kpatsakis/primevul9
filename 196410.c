nwfilterConnectListAllNWFilters(virConnectPtr conn,
                                virNWFilterPtr **nwfilters,
                                unsigned int flags)
{
    int ret;

    virCheckFlags(0, -1);

    if (virConnectListAllNWFiltersEnsureACL(conn) < 0)
        return -1;

    nwfilterDriverLock();
    ret = virNWFilterObjListExport(conn, driver->nwfilters, nwfilters,
                                   virConnectListAllNWFiltersCheckACL);
    nwfilterDriverUnlock();

    return ret;
}