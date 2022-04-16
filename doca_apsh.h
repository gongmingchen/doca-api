/*
 * Copyright (C) 2021 Mellanox Technologies, Ltd. ALL RIGHTS RESERVED.
 *
 * This software product is a proprietary product of Mellanox Technologies Ltd.
 * (the "Company") and all right, title, and interest in and to the software
 * product, including all associated intellectual property rights, are and
 * shall remain exclusively with the Company.
 *
 * This software product is governed by the End User License Agreement
 * provided with the software product.
 *
 */

/**
 * @file doca_apsh.h
 * @page apsh
 * @defgroup DOCA_APSH APSH
 *
 * TODO explanation for APSH
 *
 * @{
 */

#ifndef _DOCA_APSH__H_
#define _DOCA_APSH__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "doca_apsh_attr.h"
#include "doca_compat.h"

/**
 * @brief apsh handler
 */
struct doca_apsh_ctx;

/**
 * @brief system handler
 *
 * host bare metal/vm ...
 */
struct doca_apsh_system;

/**
 * @brief system module
 *
 * for example kernel module for linux
 */
struct doca_apsh_module;

/**
 * @brief system process
 *
 * hold the process information such as pid, cmd ...
 */
struct doca_apsh_process;

/**
 * @brief system process thread
 *
 * hold the thread process information
 */
struct doca_apsh_thread;

/**
 * @brief system process libs
 *
 * hold the process loadable libraries (DLL's or SO's)
 */
struct doca_apsh_lib;

/**
 * @brief system process virtual address descriptors
 */
struct doca_apsh_vad;

/**
 * @brief system process attestation
 *
 * contain data regarding attestation - process intrusion detection
 */
struct doca_apsh_attestation;

/**
 * @brief system supported layer types
 */
enum doca_apsh_system_layer {
	DOCA_APSH_LAYER_BARE_METAL,	 	 /**< Bare metal system - no abstraction layer */
	DOCA_APSH_LAYER_VM,		 		 /**< Virtual system */
	DOCA_APSH_LAYER_DOCKER_CONTAINER /**< Docker process */
};

enum doca_apsh_system_os {
	DOCA_APSH_SYSTEM_LINUX,	  /**< linux */
	DOCA_APSH_SYSTEM_WINDOWS, /**< windows */
};

/**
 * @brief Create a new apsh handler
 *
 * Allocate memory and init the opaque struct for apsh handler.
 * Before using the system handler use doca_apsh_start
 *
 * @return
 *   apsh context required for creating system handler, NULL on failure
 */
__DOCA_EXPERIMENTAL
struct doca_apsh_ctx *doca_apsh_create(void);

/**
 * @brief Start apsh handler
 *
 * Start apsh handler and init connection to devices.
 * Need to set apsh params with setter functions before starting the system.
 * Mandatory setters: doca_apsh_dma_dev_set.
 * Other setters can be query automatically but will take time.
 *
 * @param system
 *   system handler
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_start(struct doca_apsh_ctx *ctx);

/**
 * @brief Free the APSH memory and close connections.
 *
 * @param ctx
 *   apsh context to destroy
 */
__DOCA_EXPERIMENTAL
void doca_apsh_destroy(struct doca_apsh_ctx *ctx);

/**
 * @brief Set apsh dma device
 *
 * This is a Mandatory setter
 *
 * @param ctx
 *   apsh handler
 * @param dma_dev_name
 *   device name with the capabilities of dma
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_dma_dev_set(struct doca_apsh_ctx *ctx, const char *dma_dev_name);

/**
 * @brief Set apsh regex device
 *
 * This is a Mandatory setter
 *
 * @param ctx
 *   apsh handler
 * @param regex_dev_name
 *   device name with the capabilities of regex
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_regex_dev_set(struct doca_apsh_ctx *ctx, const char *regex_dev_name);

/**
 * @brief Create a new system handler
 *
 * Allocate memory and init the opaque struct for system handler.
 * Before using the system handler use doca_apsh_system_start
 *
 * @param ctx
 *   apsh handler
 * @param type
 *   os type
 * @return
 *   returns system pointer, NULL on failure
 */
__DOCA_EXPERIMENTAL
struct doca_apsh_system *doca_apsh_system_create(struct doca_apsh_ctx *ctx);

/**
 * @brief Destroy system handler
 *
 * This will not destroy process/module/libs ...
 *
 * @param system
 *   system context to destroy
 */
__DOCA_EXPERIMENTAL
void doca_apsh_system_destroy(struct doca_apsh_system *system);

/**
 * @brief Start system handler
 *
 * Start system handler and init connection to the system.
 * Need to set system params with setter functions before starting the system.
 * Mandatory setters: os_symbol_map, mem_region, pcidev.
 * Other setters can be query automatically but will take time.
 *
 * @param system
 *   system handler
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_system_start(struct doca_apsh_system *system);

/**
 * @brief Set system os symbol map
 *
 * This is a Mandatory setter
 *
 * @param system
 *   system handler
 * @param system_os_symbol_map_path
 *   the os memory map data, uniq per os build
 *   please note that changing linux kernel (adding/removing modules) will change the map
 *   should be created by running the doca_system_os_symbol_map tool on the system os
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_sys_os_symbol_map_set(struct doca_apsh_system *system,
				    const char *system_os_symbol_map_path);

/**
 * @brief Set system allowed memory regions
 *
 * This is a Mandatory setter
 *
 * @param system
 *   system handler
 * @param system_mem_region_path
 *   path to json file containing the memory regions of the devices
 * 	 The memory regions are uniq per system, would not change on reboot or between defrent PCI
 * devices of the same system. note that adding/removing device from the host can change the
 * regions. The json can be created by running the doca_system_mem_region tool on the system.
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_sys_mem_region_set(struct doca_apsh_system *system,
				 const char *system_mem_region_path);

/**
 * @brief Set system net device
 *
 * This is a Mandatory setter
 *
 * @param system
 *   system handler
 * @param pci_index
 *   pci function index
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_sys_pcidev_set(struct doca_apsh_system *system, int pci_index);

/**
 * @brief Set system layer type
 *
 * This is a optional setter
 *
 * @param system
 *   system handler
 * @param layer_type
 *   system layer type - bare metal/vm ...
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_sys_system_layer_set(struct doca_apsh_system *system,
				   enum doca_apsh_system_layer layer_type);

/**
 * @brief Set system os type
 *
 * This is a must setter
 *
 * @param system
 *   system handler
 * @param os_type
 *   system os type - windows/linux
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_sys_os_type_set(struct doca_apsh_system *system, enum doca_apsh_system_layer os_type);

/**
 * @brief Get array of current modules installed on the system
 *
 * This function is multithreaded compatible with diffrent system context,
 * meaning do not call this function simultaneously with the same system context.
 * The return array is snapshot, this is not dynamic array, need to free it.
 *
 * @param system
 *   System handler
 * @param modules
 *   Array of module opaque pointers of the systems
 * @return
 *   Size of the array, error code on negative value.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_module_get(struct doca_apsh_system *system, struct doca_apsh_module ***modules);

/**
 * @brief Destroys a modules array
 *
 * @param modules
 * 	 Array of module opaque pointers of the systems to destroy
 */
__DOCA_EXPERIMENTAL
void doca_apsh_module_free(struct doca_apsh_module *modules);

/**
 * @brief Shadow function - get attribute value for a module
 *
 * Do not use this function, recommanded to use doca_apsh_mod_info_get
 *
 * @param module
 *   single module handler
 * @param attr
 *   Attribute to get the info on the module
 * @return
 *   return the info requested, need to cast
 */
__DOCA_EXPERIMENTAL
const void *__doca_apsh_module_info_get(struct doca_apsh_module *module,
					enum doca_apsh_module_attr attr);

/**
 * @brief Get attribute value for a module
 *
 * Get the requested info from module handler.
 * The info is right to the snapshot (at the get function moment)
 * full list (type and descriptions) can be found in doca_apsh_attr.h
 *
 * @param module
 *   single module handler
 * @param attr
 *   Attribute to get the info on the module
 * @return
 *   return the info requested (with casting)
 */
#define doca_apsh_module_info_get(module, attr) \
	((attr##_TYPE)__doca_apsh_module_info_get(module, attr))

/**
 * @brief Get array of current processes running on the system
 *
 * This function is multithreaded compatible with diffrent system context,
 * meaning do not call this function simultaneously with the same system context.
 * The return array is snapshot, this is not dynamic array, need to free it.
 *
 * @param system
 *   System handler
 * @param processes
 *   Array of process opaque pointers of the systems
 * @return
 *   Size of the array, error code on negative value.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_processes_get(struct doca_apsh_system *system, struct doca_apsh_process ***processes);

/**
 * @brief Destroys a process context
 *
 * @param processes
 * 	 Array of process opaque pointers of the systems to destroy
 */
__DOCA_EXPERIMENTAL
void doca_apsh_processes_free(struct doca_apsh_process **processes);

/**
 * @brief Shadow function - get attribute value for a process
 *
 * Do not use this function, recommanded to use doca_apsh_proc_info_get
 *
 * @param process
 *   single process handler
 * @param attr
 *   Attribute to get the info on the process
 * @return
 *   return the info requested, need to cast
 */
__DOCA_EXPERIMENTAL
const void *__doca_apsh_proc_info_get(struct doca_apsh_process *process,
				      enum doca_apsh_process_attr attr);

/**
 * @brief Get attribute value for a process
 *
 * Get the requested info from process handler.
 * The info is right to the snapshot (at the get function moment)
 * full list (type and descriptions) can be found in doca_apsh_attr.h
 *
 * @param process
 *   single process handler
 * @param attr
 *   Attribute to get the info on the module
 * @return
 *   return the info requested (with casting)
 */
#define doca_apsh_proc_info_get(process, attr) \
	((attr##_TYPE)__doca_apsh_proc_info_get(process, attr))

/**
 * @brief refresh single process handler with new snapshot
 *
 * This function is multithreaded compatible with diffrent system context,
 * Refresh the snapshot of the handler.
 * Recommended to query all wanted information before refreshing.
 *
 * @param process
 *   single process handler to refresh
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_proc_refresh(struct doca_apsh_process *process);

/**
 * @brief Get array of current process loadable libraries
 *
 * This function is multithreaded compatible with diffrent system context,
 * meaning do not call this function simultaneously with the same system context.
 * The return array is snapshot, this is not dynamic array, need to free it.
 *
 * @param process
 *   Process handler
 * @param libs
 *   Array of libs opaque pointers of the process
 * @return
 *   Size of the array, error code on negative value.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_libs_get(struct doca_apsh_process *process, struct doca_apsh_lib ***libs);

/**
 * @brief Destroys a libs context
 *
 * @param libs
 * 	 Array of libs opaque pointers of the process to destroy
 */
__DOCA_EXPERIMENTAL
void doca_apsh_libs_free(struct doca_apsh_lib **libs);

/**
 * @brief Shadow function - get attribute value for a lib
 *
 * Do not use this function, recommanded to use doca_apsh_lib_info_get
 *
 * @param lib
 *   single lib handler
 * @param attr
 *   Attribute to get the info on the lib
 * @return
 *   return the info requested, need to cast
 */
__DOCA_EXPERIMENTAL
const void *__doca_apsh_lib_info_get(struct doca_apsh_lib *lib, enum doca_apsh_lib_attr attr);

/**
 * @brief Get attribute value for a lib
 *
 * Get the requested info from lib handler.
 * The info is right to the snapshot (at the get function moment)
 * full list (type and descriptions) can be found in doca_apsh_attr.h
 *
 * @param lib
 *   single lib handler
 * @param attr
 *   Attribute to get the info on the module
 * @return
 *   return the info requested (with casting)
 */
#define doca_apsh_lib_info_get(lib, attr) ((attr##_TYPE)__doca_apsh_lib_info_get(lib, attr))

/**
 * @brief Get array of current process threads
 *
 * This function is multithreaded compatible with diffrent system context,
 * meaning do not call this function simultaneously with the same system context.
 * The return array is snapshot, this is not dynamic array, need to free it.
 *
 * @param process
 *   Process handler
 * @param threads
 *   Array of threads opaque pointers of the process
 * @return
 *   Size of the array, error code on negative value.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_threads_get(struct doca_apsh_process *process, struct doca_apsh_thread ***threads);

/**
 * @brief Destroys a threads context
 *
 * @param threads
 * 	 Array of threads opaque pointers of the process to destroy
 */
__DOCA_EXPERIMENTAL
void doca_apsh_threads_free(struct doca_apsh_thread **threads);

/**
 * @brief Shadow function - get attribute value for a thread
 *
 * Do not use this function, recommanded to use doca_apsh_thread_info_get
 *
 * @param thread
 *   single thread handler
 * @param attr
 *   Attribute to get the info on the thread
 * @return
 *   return the info requested, need to cast
 */
__DOCA_EXPERIMENTAL
const void *__doca_apsh_thread_info_get(struct doca_apsh_thread *thread,
					enum doca_apsh_lib_attr attr);

/**
 * @brief Get attribute value for a thread
 *
 * Get the requested info from thread handler.
 * The info is right to the snapshot (at the get function moment)
 * full list (type and descriptions) can be found in doca_apsh_attr.h
 *
 * @param thread
 *   single thread handler
 * @param attr
 *   Attribute to get the info on the module
 * @return
 *   return the info requested (with casting)
 */
#define doca_apsh_thread_info_get(thread, attr) \
	((attr##_TYPE)__doca_apsh_thread_info_get(thread, attr))

/**
 * @brief Get array of current process vads - virtual address descriptor
 *
 * This function is multithreaded compatible with diffrent system context,
 * meaning do not call this function simultaneously with the same system context.
 * The return array is snapshot, this is not dynamic array, need to free it.
 *
 * @param process
 *   Process handler
 * @param vads
 *   Array of vads opaque pointers of the process
 * @return
 *   Size of the array, error code on negative value.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_vads_get(struct doca_apsh_process *process, struct doca_apsh_vad ***vads);

/**
 * @brief Destroys a vads context
 *
 * @param vads
 * 	 Array of vads opaque pointers of the process to destroy
 */
__DOCA_EXPERIMENTAL
void doca_apsh_vads_free(struct doca_apsh_vad **vads);

/**
 * @brief Shadow function - get attribute value for a vad
 *
 * Do not use this function, recommanded to use doca_apsh_vad_info_get
 *
 * @param vad
 *   single vad handler
 * @param attr
 *   Attribute to get the info on the vad
 * @return
 *   return the info requested, need to cast
 */
__DOCA_EXPERIMENTAL
const void *__doca_apsh_vad_info_get(struct doca_apsh_vad *vad, enum doca_apsh_lib_attr attr);

/**
 * @brief Get attribute value for a vad
 *
 * Get the requested info from vad handler.
 * The info is right to the snapshot (at the get function moment)
 * full list (type and descriptions) can be found in doca_apsh_attr.h
 *
 * @param vad
 *   single vad handler
 * @param attr
 *   Attribute to get the info on the module
 * @return
 *   return the info requested (with casting)
 */
#define doca_apsh_vad_info_get(vad, attr) ((attr##_TYPE)__doca_apsh_vad_info_get(vad, attr))

/**
 * @brief Get current process attestation
 *
 * This function is multithreaded compatible with diffrent system context,
 * meaning do not call this function simultaneously with the same system context.
 * The return is snapshot, this is not dynamic, need to free it.
 *
 * @param process
 *   Process handler
 * @param exec_hash_map_path
 *   path to file containing the hash calculations of the executable and dlls/libs of the process
 *   note that changing the process code or any libs can effect this.
 *   The file can be created by running the doca_exec_hash_build_map tool on the system.
 * @param attestation
 *   Attestation opaque pointers of the process
 * @return
 *   Size of the array, error code on negative value.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_attestation_get(struct doca_apsh_process *process, const char *exec_hash_map_path,
			      struct doca_apsh_attestation ***attestation);

/**
 * @brief Destroys a attestation context
 *
 * @param attestation
 * 	 Attestation opaque pointer of the process to destroy
 */
__DOCA_EXPERIMENTAL
void doca_apsh_attestation_free(struct doca_apsh_attestation **attestation);

/**
 * @brief Shadow function - get attribute value for a attestation
 *
 * Do not use this function, recommanded to use doca_apsh_attestation_info_get
 *
 * @param attestation
 *   single attestation handler
 * @param attr
 *   Attribute to get the info on the attestation
 * @return
 *   return the info requested, need to cast
 */
__DOCA_EXPERIMENTAL
const void *__doca_apsh_attst_info_get(struct doca_apsh_attestation *attestation,
				       enum doca_apsh_attestation_attr attr);

/**
 * @brief Get attribute value for a attestation
 *
 * Get the requested info from attestation handler.
 * The info is right to the snapshot (at the get function moment)
 * full list (type and descriptions) can be found in doca_apsh_attr.h
 *
 * @param attestation
 *   single attestation handler
 * @param attr
 *   Attribute to get the info on the module
 * @return
 *   return the info requested (with casting)
 */
#define doca_apsh_attst_info_get(attestation, attr) \
	((attr##_TYPE)__doca_apsh_attst_info_get(attestation, attr))

/**
 * @brief refresh single attestation handler of a process with new snapshot
 *
 * This function is multithreaded compatible with diffrent system context,
 * Refresh the snapshot of the handler.
 * Recommended to query all wanted information before refreshing.
 *
 * @param attestation
 *   single attestation handler to refresh
 * @return
 *   Size of the array, error code on negative value.
 */
__DOCA_EXPERIMENTAL
int doca_apsh_attst_refresh(struct doca_apsh_attestation ***attestation);

#ifdef __cplusplus
}
#endif

/** @} */

#endif