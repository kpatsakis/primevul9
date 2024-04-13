void js_dumpvalue(js_State *J, js_Value v)
{
	minify = 0;
	switch (v.type) {
	case JS_TUNDEFINED: printf("undefined"); break;
	case JS_TNULL: printf("null"); break;
	case JS_TBOOLEAN: printf(v.u.boolean ? "true" : "false"); break;
	case JS_TNUMBER: printf("%.9g", v.u.number); break;
	case JS_TSHRSTR: printf("'%s'", v.u.shrstr); break;
	case JS_TLITSTR: printf("'%s'", v.u.litstr); break;
	case JS_TMEMSTR: printf("'%s'", v.u.memstr->p); break;
	case JS_TOBJECT:
		if (v.u.object == J->G) {
			printf("[Global]");
			break;
		}
		switch (v.u.object->type) {
		case JS_COBJECT: printf("[Object %p]", (void*)v.u.object); break;
		case JS_CARRAY: printf("[Array %p]", (void*)v.u.object); break;
		case JS_CFUNCTION:
			printf("[Function %p, %s, %s:%d]",
				(void*)v.u.object,
				v.u.object->u.f.function->name,
				v.u.object->u.f.function->filename,
				v.u.object->u.f.function->line);
			break;
		case JS_CSCRIPT: printf("[Script %s]", v.u.object->u.f.function->filename); break;
		case JS_CCFUNCTION: printf("[CFunction %s]", v.u.object->u.c.name); break;
		case JS_CBOOLEAN: printf("[Boolean %d]", v.u.object->u.boolean); break;
		case JS_CNUMBER: printf("[Number %g]", v.u.object->u.number); break;
		case JS_CSTRING: printf("[String'%s']", v.u.object->u.s.string); break;
		case JS_CERROR: printf("[Error]"); break;
		case JS_CARGUMENTS: printf("[Arguments %p]", (void*)v.u.object); break;
		case JS_CITERATOR: printf("[Iterator %p]", (void*)v.u.object); break;
		case JS_CUSERDATA:
			printf("[Userdata %s %p]", v.u.object->u.user.tag, v.u.object->u.user.data);
			break;
		default: printf("[Object %p]", (void*)v.u.object); break;
		}
		break;
	}
}