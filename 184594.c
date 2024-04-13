jint JNICALL JNI_GetDefaultJavaVMInitArgs(void *vm_args) {
	UDATA requestedVersion = (UDATA)((JDK1_1InitArgs *)vm_args)->version;
	
	switch (requestedVersion) {
	case JNI_VERSION_1_2:
	case JNI_VERSION_1_4:
	case JNI_VERSION_1_6:
	case JNI_VERSION_1_8:
	case JNI_VERSION_9:
	case JNI_VERSION_10:
		return JNI_OK;
	}
	
	return JNI_EVERSION;	
}