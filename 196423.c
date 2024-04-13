nwfilterConnectListAllNWFilterBindings(virConnectPtr conn,
                                       virNWFilterBindingPtr **bindings,
                                       unsigned int flags)
{
    virCheckFlags(0, -1);

    if (virConnectListAllNWFilterBindingsEnsureACL(conn) < 0)
        return -1;

    return virNWFilterBindingObjListExport(driver->bindings, conn, bindings,
                                           virConnectListAllNWFilterBindingsCheckACL);
}