ves_icall_System_Reflection_Assembly_GetManifestModuleInternal (MonoReflectionAssembly *assembly) 
{
	return mono_module_get_object (mono_object_domain (assembly), assembly->assembly->image);
}