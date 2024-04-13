static int wait_consider_task(struct wait_opts *wo, int ptrace,
				struct task_struct *p)
{
	/*
	 * We can race with wait_task_zombie() from another thread.
	 * Ensure that EXIT_ZOMBIE -> EXIT_DEAD/EXIT_TRACE transition
	 * can't confuse the checks below.
	 */
	int exit_state = READ_ONCE(p->exit_state);
	int ret;

	if (unlikely(exit_state == EXIT_DEAD))
		return 0;

	ret = eligible_child(wo, ptrace, p);
	if (!ret)
		return ret;

	if (unlikely(exit_state == EXIT_TRACE)) {
		/*
		 * ptrace == 0 means we are the natural parent. In this case
		 * we should clear notask_error, debugger will notify us.
		 */
		if (likely(!ptrace))
			wo->notask_error = 0;
		return 0;
	}

	if (likely(!ptrace) && unlikely(p->ptrace)) {
		/*
		 * If it is traced by its real parent's group, just pretend
		 * the caller is ptrace_do_wait() and reap this child if it
		 * is zombie.
		 *
		 * This also hides group stop state from real parent; otherwise
		 * a single stop can be reported twice as group and ptrace stop.
		 * If a ptracer wants to distinguish these two events for its
		 * own children it should create a separate process which takes
		 * the role of real parent.
		 */
		if (!ptrace_reparented(p))
			ptrace = 1;
	}

	/* slay zombie? */
	if (exit_state == EXIT_ZOMBIE) {
		/* we don't reap group leaders with subthreads */
		if (!delay_group_leader(p)) {
			/*
			 * A zombie ptracee is only visible to its ptracer.
			 * Notification and reaping will be cascaded to the
			 * real parent when the ptracer detaches.
			 */
			if (unlikely(ptrace) || likely(!p->ptrace))
				return wait_task_zombie(wo, p);
		}

		/*
		 * Allow access to stopped/continued state via zombie by
		 * falling through.  Clearing of notask_error is complex.
		 *
		 * When !@ptrace:
		 *
		 * If WEXITED is set, notask_error should naturally be
		 * cleared.  If not, subset of WSTOPPED|WCONTINUED is set,
		 * so, if there are live subthreads, there are events to
		 * wait for.  If all subthreads are dead, it's still safe
		 * to clear - this function will be called again in finite
		 * amount time once all the subthreads are released and
		 * will then return without clearing.
		 *
		 * When @ptrace:
		 *
		 * Stopped state is per-task and thus can't change once the
		 * target task dies.  Only continued and exited can happen.
		 * Clear notask_error if WCONTINUED | WEXITED.
		 */
		if (likely(!ptrace) || (wo->wo_flags & (WCONTINUED | WEXITED)))
			wo->notask_error = 0;
	} else {
		/*
		 * @p is alive and it's gonna stop, continue or exit, so
		 * there always is something to wait for.
		 */
		wo->notask_error = 0;
	}

	/*
	 * Wait for stopped.  Depending on @ptrace, different stopped state
	 * is used and the two don't interact with each other.
	 */
	ret = wait_task_stopped(wo, ptrace, p);
	if (ret)
		return ret;

	/*
	 * Wait for continued.  There's only one continued state and the
	 * ptracer can consume it which can confuse the real parent.  Don't
	 * use WCONTINUED from ptracer.  You don't need or want it.
	 */
	return wait_task_continued(wo, p);
}