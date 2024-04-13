static SQInteger table_map(HSQUIRRELVM v)
{
	SQObject &o = stack_get(v, 1);
	SQTable *tbl = _table(o);
	SQInteger nitr, n = 0;
	SQInteger nitems = tbl->CountUsed();
	SQObjectPtr ret = SQArray::Create(_ss(v), nitems);
	SQObjectPtr itr, key, val;
	while ((nitr = tbl->Next(false, itr, key, val)) != -1) {
		itr = (SQInteger)nitr;

		v->Push(o);
		v->Push(key);
		v->Push(val);
		if (SQ_FAILED(sq_call(v, 3, SQTrue, SQFalse))) {
			return SQ_ERROR;
		}
		_array(ret)->Set(n, v->GetUp(-1));
		v->Pop();
		n++;
	}

	v->Push(ret);
	return 1;
}