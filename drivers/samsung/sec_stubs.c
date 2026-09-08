#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/notifier.h>

/* SEC_ARGOS Stubs - only compiled when the real ARGOS driver is not enabled */
#ifndef CONFIG_ARGOS
int argos_task_affinity_setup_label(struct task_struct *p, const char *label,
				    struct cpumask *affinity_cpu_mask,
				    struct cpumask *default_cpu_mask)
{
	return 0;
}
EXPORT_SYMBOL(argos_task_affinity_setup_label);

int sec_argos_register_notifier(struct notifier_block *n, char *label)
{
	return 0;
}
EXPORT_SYMBOL(sec_argos_register_notifier);

int sec_argos_unregister_notifier(struct notifier_block *n, char *label)
{
	return 0;
}
EXPORT_SYMBOL(sec_argos_unregister_notifier);
#endif /* !CONFIG_ARGOS */

/* SEC_STI / ABC Stubs - only compiled when the real ABC driver is not enabled */
#ifndef CONFIG_SEC_ABC
void sec_abc_send_event(char *str)
{
}
EXPORT_SYMBOL(sec_abc_send_event);

int sec_abc_get_enabled(void)
{
	return 0;
}
EXPORT_SYMBOL(sec_abc_get_enabled);

int sec_abc_wait_enabled(void)
{
	return 0;
}
EXPORT_SYMBOL(sec_abc_wait_enabled);
#endif /* !CONFIG_SEC_ABC */

/* SEC_SMEM Stubs - only compiled when the real SEC_SMEM driver is not enabled */
#ifndef CONFIG_SEC_SMEM
char* get_ddr_vendor_name(void) { return "STUB"; }
EXPORT_SYMBOL(get_ddr_vendor_name);

uint32_t get_ddr_DSF_version(void) { return 0; }
EXPORT_SYMBOL(get_ddr_DSF_version);

uint8_t get_ddr_info(uint8_t type) { return 0; }
EXPORT_SYMBOL(get_ddr_info);

void sec_smem_cpuclk_log_raw(size_t slot, unsigned long rate) {}
EXPORT_SYMBOL(sec_smem_cpuclk_log_raw);

void sec_smem_clk_osm_add_log_cpufreq(unsigned int cpu, unsigned long rate, const char *name) {}
EXPORT_SYMBOL(sec_smem_clk_osm_add_log_cpufreq);

void sec_smem_clk_osm_add_log_l3(unsigned long rate) {}
EXPORT_SYMBOL(sec_smem_clk_osm_add_log_l3);
#endif /* !CONFIG_SEC_SMEM */

#ifndef CONFIG_SEC_DEBUG_TSP_LOG
void sec_tsp_sponge_log(char *buf) {}
EXPORT_SYMBOL(sec_tsp_sponge_log);

void sec_debug_tsp_log(char *fmt, ...) {}
EXPORT_SYMBOL(sec_debug_tsp_log);

void sec_debug_tsp_raw_data(char *fmt, ...) {}
EXPORT_SYMBOL(sec_debug_tsp_raw_data);

void sec_debug_tsp_log_msg(char *msg, char *fmt, ...) {}
EXPORT_SYMBOL(sec_debug_tsp_log_msg);

void sec_debug_tsp_raw_data_msg(char *msg, char *fmt, ...) {}
EXPORT_SYMBOL(sec_debug_tsp_raw_data_msg);

void sec_debug_tsp_command_history(char *buf) {}
EXPORT_SYMBOL(sec_debug_tsp_command_history);
#endif
