ves_icall_Remoting_RealProxy_GetTransparentProxy (MonoObject *this, MonoString *class_name)
{
	MonoDomain *domain = mono_object_domain (this); 
	MonoObject *res;
	MonoRealProxy *rp = ((MonoRealProxy *)this);
	MonoTransparentProxy *tp;
	MonoType *type;
	MonoClass *klass;

	MONO_ARCH_SAVE_REGS;

	res = mono_object_new (domain, mono_defaults.transparent_proxy_class);
	tp = (MonoTransparentProxy*) res;
	
	MONO_OBJECT_SETREF (tp, rp, rp);
	type = ((MonoReflectionType *)rp->class_to_proxy)->type;
	klass = mono_class_from_mono_type (type);

	tp->custom_type_info = (mono_object_isinst (this, mono_defaults.iremotingtypeinfo_class) != NULL);
	tp->remote_class = mono_remote_class (domain, class_name, klass);

	res->vtable = mono_remote_class_vtable (domain, tp->remote_class, rp);
	return res;
}