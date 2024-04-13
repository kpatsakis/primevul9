static inline To bitwise_cast(From from) {
  BOOST_STATIC_ASSERT(sizeof(From) == sizeof(To));

  // BAD!!!  These are all broken with -O2.
  //return *reinterpret_cast<To*>(&from);  // BAD!!!
  //return *static_cast<To*>(static_cast<void*>(&from));  // BAD!!!
  //return *(To*)(void*)&from;  // BAD!!!

  // Super clean and paritally blessed by section 3.9 of the standard.
  //unsigned char c[sizeof(from)];
  //memcpy(c, &from, sizeof(from));
  //To to;
  //memcpy(&to, c, sizeof(c));
  //return to;

  // Slightly more questionable.
  // Same code emitted by GCC.
  //To to;
  //memcpy(&to, &from, sizeof(from));
  //return to;

  // Technically undefined, but almost universally supported,
  // and the most efficient implementation.
  union {
    From f;
    To t;
  } u;
  u.f = from;
  return u.t;
}