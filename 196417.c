nwfilterBindingLookupByPortDev(virConnectPtr conn,
                               const char *portdev)
{
    virNWFilterBindingPtr ret = NULL;
    virNWFilterBindingObj *obj;
    virNWFilterBindingDef *def;

    obj = virNWFilterBindingObjListFindByPortDev(driver->bindings,
                                                 portdev);
    if (!obj) {
        virReportError(VIR_ERR_NO_NWFILTER_BINDING,
                       _("no nwfilter binding for port dev '%s'"), portdev);
        goto cleanup;
    }

    def = virNWFilterBindingObjGetDef(obj);
    if (virNWFilterBindingLookupByPortDevEnsureACL(conn, def) < 0)
        goto cleanup;

    ret = virGetNWFilterBinding(conn, def->portdevname, def->filter);

 cleanup:
    virNWFilterBindingObjEndAPI(&obj);
    return ret;
}