void jsC_dumpfunction(js_State *J, js_Function *F)
{
	js_Instruction *p = F->code;
	js_Instruction *end = F->code + F->codelen;
	char *s;
	double n;
	int i;

	minify = 0;

	printf("%s(%d)\n", F->name, F->numparams);
	if (F->strict) printf("\tstrict\n");
	if (F->lightweight) printf("\tlightweight\n");
	if (F->arguments) printf("\targuments\n");
	printf("\tsource %s:%d\n", F->filename, F->line);
	for (i = 0; i < F->funlen; ++i)
		printf("\tfunction %d %s\n", i, F->funtab[i]->name);
	for (i = 0; i < F->varlen; ++i)
		printf("\tlocal %d %s\n", i + 1, F->vartab[i]);

	printf("{\n");
	while (p < end) {
		int ln = *p++;
		int c = *p++;

		printf("%5d(%3d): ", (int)(p - F->code) - 2, ln);
		ps(opname[c]);

		switch (c) {
		case OP_INTEGER:
			printf(" %ld", (long)((*p++) - 32768));
			break;
		case OP_NUMBER:
			memcpy(&n, p, sizeof(n));
			p += sizeof(n) / sizeof(*p);
			printf(" %.9g", n);
			break;
		case OP_STRING:
			memcpy(&s, p, sizeof(s));
			p += sizeof(s) / sizeof(*p);
			pc(' ');
			pstr(s);
			break;
		case OP_NEWREGEXP:
			pc(' ');
			memcpy(&s, p, sizeof(s));
			p += sizeof(s) / sizeof(*p);
			pregexp(s, *p++);
			break;

		case OP_GETVAR:
		case OP_HASVAR:
		case OP_SETVAR:
		case OP_DELVAR:
		case OP_GETPROP_S:
		case OP_SETPROP_S:
		case OP_DELPROP_S:
		case OP_CATCH:
			memcpy(&s, p, sizeof(s));
			p += sizeof(s) / sizeof(*p);
			pc(' ');
			ps(s);
			break;

		case OP_GETLOCAL:
		case OP_SETLOCAL:
		case OP_DELLOCAL:
			printf(" %s", F->vartab[*p++ - 1]);
			break;

		case OP_CLOSURE:
		case OP_CALL:
		case OP_NEW:
		case OP_JUMP:
		case OP_JTRUE:
		case OP_JFALSE:
		case OP_JCASE:
		case OP_TRY:
			printf(" %ld", (long)*p++);
			break;
		}

		nl();
	}
	printf("}\n");

	for (i = 0; i < F->funlen; ++i) {
		if (F->funtab[i] != F) {
			printf("function %d ", i);
			jsC_dumpfunction(J, F->funtab[i]);
		}
	}
}