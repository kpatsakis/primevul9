JVM_DefineClassWithSource(JNIEnv *env, const char * className, jobject classLoader, const jbyte * classArray, jsize length, jobject domain, const char * source)
{
	J9VMThread* currentThread = (J9VMThread*) env;
	J9JavaVM*  vm = currentThread->javaVM;
	J9InternalVMFunctions *vmFuncs = vm->internalVMFunctions;
	J9ClassLoader* vmLoader;
	j9object_t loaderObject;
	jstring classNameString;

	classNameString = (*env)->NewStringUTF(env,className);

	vmFuncs->internalEnterVMFromJNI(currentThread);

	if (CLASSNAME_INVALID == vmFuncs->verifyQualifiedName(currentThread, J9_JNI_UNWRAP_REFERENCE(classNameString))) {
		vmFuncs->setCurrentException(currentThread, J9VMCONSTANTPOOL_JAVALANGNOCLASSDEFFOUNDERROR, (UDATA *)*(j9object_t*)classNameString);
		vmFuncs->internalExitVMToJNI(currentThread);
		return NULL;
	}

	loaderObject = J9_JNI_UNWRAP_REFERENCE(classLoader);
	vmLoader = J9VMJAVALANGCLASSLOADER_VMREF(currentThread, loaderObject);
	if (NULL == vmLoader) {
		vmLoader = vmFuncs->internalAllocateClassLoader(vm, loaderObject);
		if (NULL == vmLoader) {
			vmFuncs->internalExitVMToJNI(currentThread);
			return NULL;
		}
	}
	vmFuncs->internalExitVMToJNI(currentThread);
	return jvmDefineClassHelper(env, classLoader, classNameString, (jbyte*)classArray, 0, length, domain, 0);
}