coolkey_get_fixed_boolean_bit(CK_ATTRIBUTE_TYPE type)
{
	switch(type) {
	case CKA_TOKEN:               return 0x00000080;
	case CKA_PRIVATE:             return 0x00000100;
	case CKA_MODIFIABLE:          return 0x00000200;
	case CKA_DERIVE:              return 0x00000400;
	case CKA_LOCAL:               return 0x00000800;
	case CKA_ENCRYPT:             return 0x00001000;
	case CKA_DECRYPT:             return 0x00002000;
	case CKA_WRAP:                return 0x00004000;
	case CKA_UNWRAP:              return 0x00008000;
	case CKA_SIGN:                return 0x00010000;
	case CKA_SIGN_RECOVER:        return 0x00020000;
	case CKA_VERIFY:              return 0x00040000;
	case CKA_VERIFY_RECOVER:      return 0x00080000;
	case CKA_SENSITIVE:           return 0x00100000;
	case CKA_ALWAYS_SENSITIVE:    return 0x00200000;
	case CKA_EXTRACTABLE:         return 0x00400000;
	case CKA_NEVER_EXTRACTABLE:   return 0x00800000;
	default: break;
	}
	return 0; /* return no bits */
}