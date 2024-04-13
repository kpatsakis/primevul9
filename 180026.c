ves_icall_System_Reflection_Module_GetMDStreamVersion (MonoImage *image)
{
	return (image->md_version_major << 16) | (image->md_version_minor);
}