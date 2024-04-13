const char *ldb_strerror(int ldb_err)
{
	switch (ldb_err) {
	case LDB_SUCCESS:
		return "Success";
	case LDB_ERR_OPERATIONS_ERROR:
		return "Operations error";
	case LDB_ERR_PROTOCOL_ERROR:
		return "Protocol error";
	case LDB_ERR_TIME_LIMIT_EXCEEDED:
		return "Time limit exceeded";
	case LDB_ERR_SIZE_LIMIT_EXCEEDED:
		return "Size limit exceeded";
	case LDB_ERR_COMPARE_FALSE:
		return "Compare false";
	case LDB_ERR_COMPARE_TRUE:
		return "Compare true";
	case LDB_ERR_AUTH_METHOD_NOT_SUPPORTED:
		return "Auth method not supported";
	case LDB_ERR_STRONG_AUTH_REQUIRED:
		return "Strong auth required";
/* 9 RESERVED */
	case LDB_ERR_REFERRAL:
		return "Referral error";
	case LDB_ERR_ADMIN_LIMIT_EXCEEDED:
		return "Admin limit exceeded";
	case LDB_ERR_UNSUPPORTED_CRITICAL_EXTENSION:
		return "Unsupported critical extension";
	case LDB_ERR_CONFIDENTIALITY_REQUIRED:
		return "Confidentiality required";
	case LDB_ERR_SASL_BIND_IN_PROGRESS:
		return "SASL bind in progress";
	case LDB_ERR_NO_SUCH_ATTRIBUTE:
		return "No such attribute";
	case LDB_ERR_UNDEFINED_ATTRIBUTE_TYPE:
		return "Undefined attribute type";
	case LDB_ERR_INAPPROPRIATE_MATCHING:
		return "Inappropriate matching";
	case LDB_ERR_CONSTRAINT_VIOLATION:
		return "Constraint violation";
	case LDB_ERR_ATTRIBUTE_OR_VALUE_EXISTS:
		return "Attribute or value exists";
	case LDB_ERR_INVALID_ATTRIBUTE_SYNTAX:
		return "Invalid attribute syntax";
/* 22-31 unused */
	case LDB_ERR_NO_SUCH_OBJECT:
		return "No such object";
	case LDB_ERR_ALIAS_PROBLEM:
		return "Alias problem";
	case LDB_ERR_INVALID_DN_SYNTAX:
		return "Invalid DN syntax";
/* 35 RESERVED */
	case LDB_ERR_ALIAS_DEREFERENCING_PROBLEM:
		return "Alias dereferencing problem";
/* 37-47 unused */
	case LDB_ERR_INAPPROPRIATE_AUTHENTICATION:
		return "Inappropriate authentication";
	case LDB_ERR_INVALID_CREDENTIALS:
		return "Invalid credentials";
	case LDB_ERR_INSUFFICIENT_ACCESS_RIGHTS:
		return "insufficient access rights";
	case LDB_ERR_BUSY:
		return "Busy";
	case LDB_ERR_UNAVAILABLE:
		return "Unavailable";
	case LDB_ERR_UNWILLING_TO_PERFORM:
		return "Unwilling to perform";
	case LDB_ERR_LOOP_DETECT:
		return "Loop detect";
/* 55-63 unused */
	case LDB_ERR_NAMING_VIOLATION:
		return "Naming violation";
	case LDB_ERR_OBJECT_CLASS_VIOLATION:
		return "Object class violation";
	case LDB_ERR_NOT_ALLOWED_ON_NON_LEAF:
		return "Not allowed on non-leaf";
	case LDB_ERR_NOT_ALLOWED_ON_RDN:
		return "Not allowed on RDN";
	case LDB_ERR_ENTRY_ALREADY_EXISTS:
		return "Entry already exists";
	case LDB_ERR_OBJECT_CLASS_MODS_PROHIBITED:
		return "Object class mods prohibited";
/* 70 RESERVED FOR CLDAP */
	case LDB_ERR_AFFECTS_MULTIPLE_DSAS:
		return "Affects multiple DSAs";
/* 72-79 unused */
	case LDB_ERR_OTHER:
		return "Other";
	}

	return "Unknown error";
}