
#ifdef CONFIG_SCHED_TUNE

#include <linux/reciprocal_div.h>

/*
 * System energy normalization constants
 */
struct target_nrg {
	unsigned long min_power;
	unsigned long max_power;
	struct reciprocal_value rdiv;
};

int schedtune_cpu_boost_with(int cpu, struct task_struct *p);
int schedtune_task_boost(struct task_struct *tsk);

int schedtune_prefer_idle(struct task_struct *tsk);

void schedtune_enqueue_task(struct task_struct *p, int cpu);
void schedtune_dequeue_task(struct task_struct *p, int cpu);

#else /* CONFIG_SCHED_TUNE */

#define schedtune_cpu_boost_with(cpu, p)  0
#define schedtune_task_boost(tsk) 0

#define schedtune_prefer_idle(tsk) 0

#define schedtune_enqueue_task(task, cpu) do { } while (0)
#define schedtune_dequeue_task(task, cpu) do { } while (0)

#define stune_util(cpu, other_util, walt_load) cpu_util_cfs(cpu_rq(cpu))
#endif /* CONFIG_SCHED_TUNE */

/*
 * Identifies threads that directly participate in UI frame delivery.
 * Used to apply extra scheduling priority independent of cgroup membership,
 * since top-app cgroup includes unrelated services (keyboard, cameraserver).
 *
 * Note: task_struct->comm is at most TASK_COMM_LEN-1 (15) chars.
 */
static inline bool is_ui_thread_name(struct task_struct *p)
{
	const char *comm = p->comm;
	char c = comm[0];

	/* Fast-fail filter: only perform strcmp if first char matches a UI thread name */
	if (c != 'R' && c != 's' && c != 'a' && c != 'H' && c != 'd' && c != 'T' && c != 'I' &&
	    c != 'n' && c != 'w' && c != 'A' && c != 'N' && c != 'S')
		return false;

	return !strcmp(comm, "RenderThread")    ||
	       !strcmp(comm, "RenderEngine")    ||
	       !strcmp(comm, "surfaceflinger")  ||
	       !strcmp(comm, "system_server")   ||
	       !strcmp(comm, "ndroid.systemui") ||
	       !strcmp(comm, "wmshell.main")    ||
	       !strcmp(comm, "wmshell.anim")    ||
	       !strcmp(comm, "android.display") ||
	       !strcmp(comm, "android.anim")    ||
	       !strcmp(comm, "android.ui")      ||
	       !strcmp(comm, "InsetsAnimation") ||
	       !strcmp(comm, "InteractionJank") ||
	       !strcmp(comm, "ActivityManager") ||
	       !strcmp(comm, "AsyncLayoutInfl") ||
	       !strcmp(comm, "NotifInflation")  ||
	       !strcmp(comm, "SysUiBg")         ||
	       !strcmp(comm, "ImageWallpaper")  ||
	       !strcmp(comm, "ScreenDecoratio") ||
	       !strncmp(comm, "HwBinder", 8)    ||
	       !strncmp(comm, "droid.launcher", 14) ||
	       !strcmp(comm, "TASKBAR_UI_THRE") ||
	       !strcmp(comm, "InputReader")     ||
	       !strcmp(comm, "InputDispatcher");
}

static inline bool is_ui_thread(struct task_struct *p)
{
#ifdef CONFIG_SCHED_WALT
	return p->is_ui;
#else
	return false;
#endif
}

