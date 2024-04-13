ves_icall_System_Reflection_Module_GetPEKind (MonoImage *image, gint32 *pe_kind, gint32 *machine)
{
	if (image->dynamic) {
		MonoDynamicImage *dyn = (MonoDynamicImage*)image;
		*pe_kind = dyn->pe_kind;
		*machine = dyn->machine;
	}
	else {
		*pe_kind = ((MonoCLIImageInfo*)(image->image_info))->cli_cli_header.ch_flags & 0x3;
		*machine = ((MonoCLIImageInfo*)(image->image_info))->cli_header.coff.coff_machine;
	}
}