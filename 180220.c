ves_icall_System_Reflection_Module_GetHINSTANCE (MonoReflectionModule *module)
{
#ifdef PLATFORM_WIN32
	if (module->image && module->image->is_module_handle)
		return module->image->raw_data;
#endif

	return (gpointer) (-1);
}