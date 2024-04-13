_dbus_atomic_dec (DBusAtomic *atomic)
{
#if DBUS_USE_SYNC
  return __sync_sub_and_fetch(&atomic->value, 1)+1;
#else
  dbus_int32_t res;

  pthread_mutex_lock (&atomic_mutex);
  res = atomic->value;
  atomic->value -= 1;
  pthread_mutex_unlock (&atomic_mutex);

  return res;
#endif
}