#ifndef _LINUX_KERNELSU_H
#define _LINUX_KERNELSU_H

#include <linux/types.h>
#include <linux/capability.h>

struct file;
struct stat;
struct stat64;
struct filename;
struct user_arg_ptr;

#ifdef CONFIG_KSU

extern bool ksu_vfs_read_hook;
extern bool ksu_su_compat_enabled;
extern bool ksu_execveat_hook;

extern bool __ksu_is_allow_uid_for_current(uid_t uid);
#define ksu_is_allow_uid_for_current(uid) unlikely(__ksu_is_allow_uid_for_current(uid))

extern int ksu_handle_faccessat(int *dfd, const char __user **filename_ptr, int *mode, int *flags);
extern int ksu_handle_vfs_read(struct file **file_ptr, char __user **buf_ptr, size_t *count_ptr, loff_t **pos_ptr);
extern int ksu_handle_execveat_ksud(int *fd, struct filename **filename_ptr, struct user_arg_ptr *argv, struct user_arg_ptr *envp, int *flags);
extern int ksu_handle_execveat_sucompat(int *fd, struct filename **filename_ptr, void *argv, void *envp, int *flags);
extern long ksu_handle_execve_sucompat(const char __user **filename_ptr, int orig_nr, const struct pt_regs *regs);
extern void ksu_handle_execve_ksud(const char __user **filename_ptr, void *argv);
extern void ksu_handle_newfstat_ret(unsigned int *fd, struct stat __user **statbuf_ptr);
extern void ksu_handle_fstat64_ret(unsigned long *fd, struct stat64 __user **statbuf_ptr);
extern void ksu_handle_input_handle_event(unsigned int *type, unsigned int *code, int *value);
extern void ksu_handle_slow_avc_audit(u32 *tsid);
extern int ksu_handle_sys_reboot(int magic1, int magic2, unsigned int cmd, void __user **arg);
extern int ksu_handle_setresuid(uid_t ruid, uid_t euid, uid_t suid);
extern void ksu_handle_capget(kernel_cap_t *permitted, kernel_cap_t *inheritable, kernel_cap_t *effective);
extern int ksu_handle_stat(int *dfd, const char __user **filename_user, int *flags);

#else

#define ksu_vfs_read_hook false
#define ksu_su_compat_enabled false
#define ksu_execveat_hook false

static inline int ksu_handle_faccessat(int *dfd, const char __user **filename_ptr, int *mode, int *flags) { return 0; }
static inline int ksu_handle_vfs_read(struct file **file_ptr, char __user **buf_ptr, size_t *count_ptr, loff_t **pos_ptr) { return 0; }
static inline int ksu_handle_execveat_ksud(int *fd, struct filename **filename_ptr, struct user_arg_ptr *argv, struct user_arg_ptr *envp, int *flags) { return 0; }
static inline int ksu_handle_execveat_sucompat(int *fd, struct filename **filename_ptr, void *argv, void *envp, int *flags) { return 0; }
static inline long ksu_handle_execve_sucompat(const char __user **filename_ptr, int orig_nr, const struct pt_regs *regs) { return 0; }
static inline void ksu_handle_execve_ksud(const char __user **filename_ptr, void *argv) {}
static inline void ksu_handle_newfstat_ret(unsigned int *fd, struct stat __user **statbuf_ptr) {}
static inline void ksu_handle_fstat64_ret(unsigned long *fd, struct stat64 __user **statbuf_ptr) {}
static inline void ksu_handle_input_handle_event(unsigned int *type, unsigned int *code, int *value) {}
static inline void ksu_handle_slow_avc_audit(u32 *tsid) {}
static inline int ksu_handle_sys_reboot(int magic1, int magic2, unsigned int cmd, void __user **arg) { return 0; }
static inline int ksu_handle_setresuid(uid_t ruid, uid_t euid, uid_t suid) { return 0; }
static inline void ksu_handle_capget(kernel_cap_t *permitted, kernel_cap_t *inheritable, kernel_cap_t *effective) {}
static inline int ksu_handle_stat(int *dfd, const char __user **filename_user, int *flags) { return 0; }
static inline bool ksu_is_allow_uid_for_current(uid_t uid) { return false; }

#endif /* CONFIG_KSU */

#endif /* _LINUX_KERNELSU_H */
