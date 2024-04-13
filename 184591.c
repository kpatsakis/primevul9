static jint initializeReflectionGlobals(JNIEnv * env, BOOLEAN includeAccessors) {
	J9VMThread *vmThread = (J9VMThread *) env;
	J9JavaVM * vm = vmThread->javaVM;
	jclass clazz, clazzConstructorAccessorImpl, clazzMethodAccessorImpl;

	clazz = (*env)->FindClass(env, "java/lang/Class");
	if (!clazz) {
		return JNI_ERR;
	}

	jlClass = (*env)->NewGlobalRef(env, clazz);
	if (!jlClass) {
		return JNI_ERR;
	}

	if (J2SE_SHAPE_RAW != J2SE_SHAPE(vm)) {
		classDepthMID = (*env)->GetStaticMethodID(env, clazz, "classDepth", "(Ljava/lang/String;)I");
		if (!classDepthMID) {
			return JNI_ERR;
		}

		classLoaderDepthMID = (*env)->GetStaticMethodID(env, clazz, "classLoaderDepth", "()I");
		if (!classLoaderDepthMID) {
			return JNI_ERR;
		}

		currentClassLoaderMID = (*env)->GetStaticMethodID(env, clazz, "currentClassLoader", "()Ljava/lang/ClassLoader;");
		if (!currentClassLoaderMID) {
			return JNI_ERR;
		}

		currentLoadedClassMID = (*env)->GetStaticMethodID(env, clazz, "currentLoadedClass", "()Ljava/lang/Class;");
		if (!currentLoadedClassMID) {
			return JNI_ERR;
		}
	}

	getNameMID = (*env)->GetMethodID(env, clazz, "getName", "()Ljava/lang/String;");
	if (!getNameMID) {
		return JNI_ERR;
	}

	clazz = (*env)->FindClass(env, "java/lang/Thread");
	if (!clazz) {
		return JNI_ERR;
	}

	jlThread = (*env)->NewGlobalRef(env, clazz);
	if (!jlThread) {
		return JNI_ERR;
	}

	sleepMID = (*env)->GetStaticMethodID(env, clazz, "sleep", "(J)V");
	if (!sleepMID) {
		return JNI_ERR;
	}

	clazz = (*env)->FindClass(env, "java/lang/Object");
	if (!clazz) {
		return JNI_ERR;
	}

	waitMID = (*env)->GetMethodID(env, clazz, "wait", "(J)V");
	if (!waitMID) {
		return JNI_ERR;
	}

	notifyMID = (*env)->GetMethodID(env, clazz, "notify", "()V");
	if (!notifyMID) {
		return JNI_ERR;
	}

	notifyAllMID = (*env)->GetMethodID(env, clazz, "notifyAll", "()V");
	if (!notifyAllMID) {
		return JNI_ERR;
	}

	if (includeAccessors) {
		if (J2SE_VERSION(vm) >= J2SE_19) {
			clazzConstructorAccessorImpl = (*env)->FindClass(env, "jdk/internal/reflect/ConstructorAccessorImpl");
			clazzMethodAccessorImpl = (*env)->FindClass(env, "jdk/internal/reflect/MethodAccessorImpl");
		} else {
			clazzConstructorAccessorImpl = (*env)->FindClass(env, "sun/reflect/ConstructorAccessorImpl");
			clazzMethodAccessorImpl = (*env)->FindClass(env, "sun/reflect/MethodAccessorImpl");
		}
		if ( (NULL == clazzConstructorAccessorImpl) || (NULL == clazzMethodAccessorImpl))	{
			return JNI_ERR;
		}
		vm->srConstructorAccessor = (*env)->NewGlobalRef(env, clazzConstructorAccessorImpl);
		if (NULL == vm->srConstructorAccessor) {
			return JNI_ERR;
		}
		vm->srMethodAccessor = (*env)->NewGlobalRef(env, clazzMethodAccessorImpl);
		if (NULL == vm->srMethodAccessor) {
			return JNI_ERR;
		}
	}
	return JNI_OK;
}