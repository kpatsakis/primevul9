nwfilterConnectListNWFilters(virConnectPtr conn,
                             char **const names,
                             int maxnames)
{
    int nnames;

    if (virConnectListNWFiltersEnsureACL(conn) < 0)
        return -1;

    nwfilterDriverLock();
    nnames = virNWFilterObjListGetNames(driver->nwfilters, conn,
                                    virConnectListNWFiltersCheckACL,
                                    names, maxnames);
    nwfilterDriverUnlock();
    return nnames;
}