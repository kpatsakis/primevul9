nwfilterBindingCreateXML(virConnectPtr conn,
                         const char *xml,
                         unsigned int flags)
{
    virNWFilterBindingDef *def;
    virNWFilterBindingObj *obj = NULL;
    virNWFilterBindingPtr ret = NULL;

    virCheckFlags(VIR_NWFILTER_BINDING_CREATE_VALIDATE, NULL);

    if (!driver->privileged) {
        virReportError(VIR_ERR_OPERATION_INVALID, "%s",
                       _("Can't define NWFilter bindings in session mode"));
        return NULL;
    }

    def = virNWFilterBindingDefParseString(xml, flags);
    if (!def)
        return NULL;

    if (virNWFilterBindingCreateXMLEnsureACL(conn, def) < 0)
        goto cleanup;

    obj = virNWFilterBindingObjListAdd(driver->bindings,
                                       def);
    if (!obj)
        goto cleanup;

    if (!(ret = virGetNWFilterBinding(conn, def->portdevname, def->filter)))
        goto cleanup;

    virNWFilterReadLockFilterUpdates();
    if (virNWFilterInstantiateFilter(driver, def) < 0) {
        virNWFilterUnlockFilterUpdates();
        virNWFilterBindingObjListRemove(driver->bindings, obj);
        g_clear_pointer(&ret, virObjectUnref);
        goto cleanup;
    }
    virNWFilterUnlockFilterUpdates();
    virNWFilterBindingObjSave(obj, driver->bindingDir);

 cleanup:
    if (!obj)
        virNWFilterBindingDefFree(def);
    virNWFilterBindingObjEndAPI(&obj);

    return ret;
}