nwfilterBindingGetXMLDesc(virNWFilterBindingPtr binding,
                          unsigned int flags)
{
    virNWFilterBindingObj *obj;
    virNWFilterBindingDef *def;
    char *ret = NULL;

    virCheckFlags(0, NULL);

    obj = virNWFilterBindingObjListFindByPortDev(driver->bindings,
                                                 binding->portdev);
    if (!obj) {
        virReportError(VIR_ERR_NO_NWFILTER_BINDING,
                       _("no nwfilter binding for port dev '%s'"), binding->portdev);
        goto cleanup;
    }

    def = virNWFilterBindingObjGetDef(obj);
    if (virNWFilterBindingGetXMLDescEnsureACL(binding->conn, def) < 0)
        goto cleanup;

    ret = virNWFilterBindingDefFormat(def);

 cleanup:
    virNWFilterBindingObjEndAPI(&obj);
    return ret;
}