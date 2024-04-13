ves_icall_Remoting_RealProxy_InternalGetProxyType (MonoTransparentProxy *tp)
{
	return mono_type_get_object (mono_object_domain (tp), &tp->remote_class->proxy_class->byval_arg);
}