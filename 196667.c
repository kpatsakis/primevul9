gboolean
mono_verifier_is_class_full_trust (MonoClass *klass)
{
	/* under CoreCLR code is trusted if it is part of the "platform" otherwise all code inside the GAC is trusted */
	gboolean trusted_location = (mono_security_get_mode () != MONO_SECURITY_MODE_CORE_CLR) ? 
		klass->image->assembly->in_gac : mono_security_core_clr_is_platform_image (klass->image);

	if (verify_all && verifier_mode == MONO_VERIFIER_MODE_OFF)
		return trusted_location || klass->image == mono_defaults.corlib;
	return verifier_mode < MONO_VERIFIER_MODE_VERIFIABLE || trusted_location || klass->image == mono_defaults.corlib;