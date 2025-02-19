nfsd4_layout_verify(struct svc_export *exp, unsigned int layout_type)
{
	if (!exp->ex_layout_types) {
		dprintk("%s: export does not support pNFS\n", __func__);
		return NULL;
	}

	if (layout_type >= LAYOUT_TYPE_MAX ||
	    !(exp->ex_layout_types & (1 << layout_type))) {
		dprintk("%s: layout type %d not supported\n",
			__func__, layout_type);
		return NULL;
	}

	return nfsd4_layout_ops[layout_type];
}