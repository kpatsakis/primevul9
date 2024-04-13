nwfilterBindingDelete(virNWFilterBindingPtr binding)
{
    virNWFilterBindingObj *obj;
    virNWFilterBindingDef *def;
    int ret = -1;

    obj = virNWFilterBindingObjListFindByPortDev(driver->bindings, binding->portdev);
    if (!obj) {
        virReportError(VIR_ERR_NO_NWFILTER_BINDING,
                       _("no nwfilter binding for port dev '%s'"), binding->portdev);
        return -1;
    }

    def = virNWFilterBindingObjGetDef(obj);
    if (virNWFilterBindingDeleteEnsureACL(binding->conn, def) < 0)
        goto cleanup;

    virNWFilterReadLockFilterUpdates();
    virNWFilterTeardownFilter(def);
    virNWFilterUnlockFilterUpdates();
    virNWFilterBindingObjDelete(obj, driver->bindingDir);
    virNWFilterBindingObjListRemove(driver->bindings, obj);

    ret = 0;

 cleanup:
    virNWFilterBindingObjEndAPI(&obj);
    return ret;
}