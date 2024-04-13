gboolean
mono_verifier_is_enabled_for_class (MonoClass *klass)
{
	return verify_all || (verifier_mode > MONO_VERIFIER_MODE_OFF && !klass->image->assembly->in_gac && klass->image != mono_defaults.corlib);