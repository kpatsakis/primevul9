create_oc(
    XOM om,
    XlcArgList args,
    int num_args)
{
    XOC oc;
    XOMGenericPart *gen = XOM_GENERIC(om);
    XOCMethodsList methods_list = oc_methods_list;
    int count;

    oc = Xcalloc(1, sizeof(XOCGenericRec));
    if (oc == NULL)
	return (XOC) NULL;

    oc->core.om = om;

    if (oc_resources[0].xrm_name == NULLQUARK)
	_XlcCompileResourceList(oc_resources, XlcNumber(oc_resources));

    if (_XlcSetValues((XPointer) oc, oc_resources, XlcNumber(oc_resources),
		      args, num_args, XlcCreateMask | XlcDefaultMask))
	goto err;

    if (oc->core.base_name_list == NULL)
	goto err;

    oc->core.resources = oc_resources;
    oc->core.num_resources = XlcNumber(oc_resources);

    if (create_fontset(oc) == False)
	goto err;

    oc->methods = &oc_generic_methods;

    if (gen->object_name) {
	count = XlcNumber(oc_methods_list);

	for ( ; count-- > 0; methods_list++) {
	    if (!_XlcCompareISOLatin1(gen->object_name, methods_list->name)) {
		oc->methods = methods_list->methods;
		break;
	    }
	}
    }

    return oc;

err:
    destroy_oc(oc);

    return (XOC) NULL;
}