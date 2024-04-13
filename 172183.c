static void hidp_session_terminate(struct hidp_session *session)
{
	atomic_inc(&session->terminate);
	wake_up_interruptible(&hidp_session_wq);
}