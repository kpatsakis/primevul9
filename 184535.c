JVM_CX8Field(JNIEnv* env, jobject obj, jfieldID field, jlong oldval, jlong newval)
{
	jboolean result = JNI_FALSE;

	Trc_SC_CX8Field_Entry(env, obj, field, oldval, newval);
	exit(230);

	return result;
}