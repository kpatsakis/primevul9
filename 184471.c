JVM_GetClassName(JNIEnv *env, jclass theClass)
{
	J9JavaVM* vm = ((J9VMThread*)env)->javaVM;
	jstring result;

	Trc_SC_GetClassName_Entry(env, theClass);

	if (J2SE_SHAPE_RAW == J2SE_SHAPE(vm)) {
		J9Class* ramClass = java_lang_Class_vmRef(env, theClass);
		J9ROMClass* romClass = ramClass->romClass;
		PORT_ACCESS_FROM_JAVAVM(vm);

		if (J9ROMCLASS_IS_ARRAY(romClass)) {
			J9ArrayClass* arrayClass = (J9ArrayClass*) ramClass;
			J9ArrayClass* elementClass = (J9ArrayClass*)arrayClass->leafComponentType;
			UDATA arity = arrayClass->arity;
			UDATA nameLength, prefixSkip;
			J9UTF8* nameUTF;
			char* name;
			UDATA finalLength;

			if (J9ROMCLASS_IS_PRIMITIVE_TYPE(elementClass->romClass)) {
				nameUTF = J9ROMCLASS_CLASSNAME(elementClass->arrayClass->romClass);
				arity -= 1;
				nameLength = arity; /* The name will have a [ in it already */
				prefixSkip = arity;
			} else {
				nameUTF = J9ROMCLASS_CLASSNAME(elementClass->romClass);
				nameLength = arity + 2; /* The semi colon and the L */
				prefixSkip = arity + 1;
			}

			finalLength = nameLength + J9UTF8_LENGTH(nameUTF) + 1;
			name = (char*)j9mem_allocate_memory(nameLength + J9UTF8_LENGTH(nameUTF) + 1, OMRMEM_CATEGORY_VM);
			if (NULL != name) {
				memset(name,'[', nameLength);
				memcpy(name+nameLength, J9UTF8_DATA(nameUTF), J9UTF8_LENGTH(nameUTF));
				name[J9UTF8_LENGTH(nameUTF)] = 0;
			}
			return NULL;
		} else {
			J9UTF8* nameUTF = J9ROMCLASS_CLASSNAME(ramClass->romClass);
			jobject result = NULL;

			char* name = (char*)j9mem_allocate_memory(J9UTF8_LENGTH(nameUTF) + 1, OMRMEM_CATEGORY_VM);
			if (NULL != name) {
				memcpy(name, J9UTF8_DATA(nameUTF), J9UTF8_LENGTH(nameUTF));
				name[J9UTF8_LENGTH(nameUTF)] = 0;
			}

			result = (*env)->NewStringUTF(env, name);
			j9mem_free_memory(name);
			return result;
		}
	}

	result = (*env)->CallObjectMethod(env, theClass, getNameMID);

	/* CMVC 95169: ensure that the result is a well defined error value if an exception occurred */
	if ((*env)->ExceptionCheck(env)) {
		result = NULL;
	}

	Trc_SC_GetClassName_Exit(env, result);

	return result;
}