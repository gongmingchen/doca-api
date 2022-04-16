/*
 * Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES, ALL RIGHTS RESERVED.
 *
 * This software product is a proprietary product of NVIDIA CORPORATION &
 * AFFILIATES (the "Company") and all right, title, and interest in and to the
 * software product, including all associated intellectual property rights, are
 * and shall remain exclusively with the Company.
 *
 * This software product is governed by the End User License Agreement
 * provided with the software product.
 *
 */

/**
 * @file doca_flow.h
 * @page doca flow
 * @defgroup Flow flow
 * DOCA HW offload flow library. For more details please refer to the user guide
 * on DOCA devzone.
 *
 * @{
 */

#ifndef _DOCA_FLOW_H_
#define _DOCA_FLOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <doca_compat.h>
#include <doca_flow_net.h>

/**
 * @brief doca flow port struct
 */
struct doca_flow_port;

/**
 * @brief doca flow pipeline struct
 */
struct doca_flow_pipe;

/**
 * @brief doca flow pipeline entry struct
 */
struct doca_flow_pipe_entry;

/**
 * @brief doca flow error type define
 */
enum doca_flow_error_type {
	DOCA_ERROR_UNKNOWN,
	/**< Unknown error */
	DOCA_ERROR_UNSUPPORTED,
	/**< Operation unsupported */
	DOCA_ERROR_INVALID_PARAM,
	/**< Invalid parameter */
	DOCA_ERROR_PIPE_BUILD_ITEM,
	/**< Build pipe match items error */
	DOCA_ERROR_PIPE_MODIFY_ITEM,
	/**< Modify pipe match items error */
	DOCA_ERROR_PIPE_BUILD_ACTION,
	/**< Build pipe actions error */
	DOCA_ERROR_PIPE_MODIFY_ACTION,
	/**< Modify pipe actions error */
	DOCA_ERROR_PIPE_BUILD_FWD,
	/**< Build pipe fwd error */
	DOCA_ERROR_FLOW_CREATE,
	/**< Flow creation error */
	DOCA_ERROR_OOM,
	/**< Out of memory */
	DOCA_ERROR_PORT,
	/**< Port error */
};

/**
 * @brief doca flow error message struct
 */
struct doca_flow_error {
	enum doca_flow_error_type type;
	/**< Cause field and error types */
	const char *message;
	/**< Human-readable error message */
};

/**
 * @brief doca flow global configuration
 */
struct doca_flow_cfg {
	uint32_t total_sessions;
	/**< total flows count */
	uint16_t queues;
	/**< queue id for each offload thread */
	bool is_hairpin;
	/**< when true, the fwd will be hairpin queue */
	bool aging;
	/**< when true, aging is handled by doca */
};

/**
 * @brief doca flow port type
 */
enum doca_flow_port_type {
	DOCA_FLOW_PORT_DPDK_BY_ID,
	/**< dpdk port by mapping id */
};

/**
 * @brief doca flow port configuration
 */
struct doca_flow_port_cfg {
	uint16_t port_id;
	/**< dpdk port id */
	enum doca_flow_port_type type;
	/**< mapping type of port */
	const char *devargs;
	/**< specific per port type cfg */
	uint16_t priv_data_size;
	/**< user private data */
};

/**
 * @brief doca flow match flags
 */
enum doca_flow_match_flags {
	DOCA_FLOW_MATCH_TCP_FIN,
	/**< match tcp packets with Fin flag */
};

/**
 * @brief doca flow matcher information
 */
struct doca_flow_match {
	uint32_t flags;
	/**< match items which are no value */
	uint8_t out_src_mac[DOCA_ETHER_ADDR_LEN];
	/**< outer source mac address */
	uint8_t out_dst_mac[DOCA_ETHER_ADDR_LEN];
	/**< outer destination mac address */
	doca_be16_t out_eth_type;
	/**< outer Ethernet layer type */
	doca_be16_t vlan_id;
	/**< outer vlan id */
	struct doca_flow_ip_addr out_src_ip;
	/**< outer source ip address */
	struct doca_flow_ip_addr out_dst_ip;
	/**< outer destination ip address */
	uint8_t out_l4_type;
	/**< outer layer 4 protocol type */
	doca_be16_t out_src_port;
	/**< outer layer 4 source port */
	doca_be16_t out_dst_port;
	/**< outer layer 4 destination port */
	struct doca_flow_tun tun;
	/**< tunnel info */
	doca_be16_t in_eth_type;
	/**< inner Ethernet layer type */
	struct doca_flow_ip_addr in_src_ip;
	/**< inner source ip address if tunnel is used */
	struct doca_flow_ip_addr in_dst_ip;
	/**< inner destination ip address if tunnel is used */
	uint8_t in_l4_type;
	/**< inner layer 4 protocol type if tunnel is used */
	doca_be16_t in_src_port;
	/**< inner layer 4 source port if tunnel is used */
	doca_be16_t in_dst_port;
	/**< inner layer 4 destination port if tunnel is used */
};

/**
 * @brief doca flow encap data information
 */
struct doca_flow_encap_action {
	uint8_t src_mac[DOCA_ETHER_ADDR_LEN];
	/**< source mac address */
	uint8_t dst_mac[DOCA_ETHER_ADDR_LEN];
	/**< destination mac address */
	struct doca_flow_ip_addr src_ip;
	/**< source ip address */
	struct doca_flow_ip_addr dst_ip;
	/**< destination ip address */
	struct doca_flow_tun tun;
	/**< tunnel info */
};

/**
 * @brief doca flow actions information
 */
struct doca_flow_actions {
	bool decap;
	/**< when true, will do decap */
	uint8_t mod_src_mac[DOCA_ETHER_ADDR_LEN];
	/**< modify source mac address */
	uint8_t mod_dst_mac[DOCA_ETHER_ADDR_LEN];
	/**< modify destination mac address */
	struct doca_flow_ip_addr mod_src_ip;
	/**< modify source ip address */
	struct doca_flow_ip_addr mod_dst_ip;
	/**< modify destination ip address */
	doca_be16_t mod_src_port;
	/**< modify layer 4 source port */
	doca_be16_t mod_dst_port;
	/**< modify layer 4 destination port */
	bool dec_ttl;
	/**< decrease TTL value */
	bool has_encap;
	/**< when true, will do encap */
	struct doca_flow_encap_action encap;
	/**< encap data information */
};

/**
 * @brief forwarding action type
 */
enum doca_flow_fwd_type {
	DOCA_FLOW_FWD_NONE = 0,
	/**< No forward action be set */
	DOCA_FLOW_FWD_RSS,
	/**< Forwards packets to rss */
	DOCA_FLOW_FWD_PORT,
	/**< Forwards packets to one port */
	DOCA_FLOW_FWD_PIPE,
	/**< Forwards packets to another pipe */
	DOCA_FLOW_FWD_DROP
	/**< Drops packets */
};

/**
 * @brief rss offload types
 */
enum doca_rss_type {
	DOCA_FLOW_RSS_IP = (1 << 0),
	/**< rss by ip head */
	DOCA_FLOW_RSS_UDP = (1 << 1),
	/**< rss by udp head */
	DOCA_FLOW_RSS_TCP = (1 << 2),
	/**< rss by tcp head */
};

/**
 * @brief forwarding configuration
 */
struct doca_flow_fwd {
	enum doca_flow_fwd_type type;
	/**< indicate the forwarding type */
	union {
		struct {
			uint32_t rss_flags;
			/**< rss offload types */
			uint16_t *rss_queues;
			/**< rss queues array */
			int num_of_queues;
			/**< number of queues */
			uint32_t rss_mark;
			/**< markid of each queues */
		};
		/**< rss configuration information */
		struct {
			uint16_t port_id;
			/**< destination port id */
		};
		/**< port configuration information */
		struct {
			struct doca_flow_pipe *next_pipe;
			/**< next pipe pointer */
		};
		/**< next pipe configuration information */
	};
};

/**
 * @brief doca monitor action flags
 */
enum {
	DOCA_FLOW_MONITOR_NONE = 0,
	/**< No monitor action be set */
	DOCA_FLOW_MONITOR_METER = (1 << 1),
	/**< set monitor with meter action */
	DOCA_FLOW_MONITOR_COUNT = (1 << 2),
	/**< set monitor with counter action */
	DOCA_FLOW_MONITOR_AGING = (1 << 3),
	/**< set monitor with aging action */
};

/**
 * @brief doca monitor action configuration
 */
struct doca_flow_monitor {
	uint8_t flags;
	/**< indicate which actions be included */
	struct {
		uint32_t id;
		/**< meter id */
		uint64_t cir;
		/**< Committed Information Rate (bytes/second). */
		uint64_t cbs;
		/** Committed Burst Size (bytes). */
	};
	/**< meter action configuration */
	uint32_t aging;
	/**< aging time in seconds.*/
	uint64_t user_data;
	/**< aging user data input.*/
};

/**
 * @brief pipeline configuration
 */
struct doca_flow_pipe_cfg {
	const char *name;
	/**< name for the pipeline */
	struct doca_flow_port *port;
	/**< port for the pipeline */
	bool is_root;
	/**< pipeline is root or not */
	struct doca_flow_match *match;
	/**< matcher for the pipeline */
	struct doca_flow_match *match_mask;
	/**< match mask for the pipeline */
	struct doca_flow_actions *actions;
	/**< actions for the pipeline */
	struct doca_flow_monitor *monitor;
	/**< monitor for the pipeline */
};

/**
 * @brief flow query result
 */
struct doca_flow_query {
	uint64_t total_bytes;
	/**< total bytes hit this flow */
	uint64_t total_pkts;
	/**< total packets hit this flow */
};

/**
 * @brief aged flow query callback context
 */
struct doca_flow_aged_query {
	uint64_t user_data;
	/**< The user input context, otherwish the doca_flow_pipe_entry pointer */
};

/**
 * @brief Initialize the doca flow.
 *
 * This is the global initialization function for doca flow. It
 * initializes all resources used by doca flow.
 *
 * Must be invoked first before any other function in this API.
 * this is a one time call, used for doca flow initialization and
 * global configurations.
 *
 * @param cfg
 * Port configuration, see doca_flow_cfg for details.
 * @param error
 * Output error, set doca_flow_error for details.
 * @return
 * 0 on success, a negative errno value otherwise and error is set.
 */
__DOCA_EXPERIMENTAL
int
doca_flow_init(const struct doca_flow_cfg *cfg,
	       struct doca_flow_error *error);

/**
 * @brief Destroy the doca flow.
 *
 * Release all the resources used by doca flow.
 *
 * Must be invoked at the end of the application, before it exits.
 */
__DOCA_EXPERIMENTAL
void
doca_flow_destroy(void);

/**
 * @brief Start a doca port.
 *
 * Start a port with the given configuration. Will create one port in
 * the doca flow layer, allocate all resources used by this port, and
 * create the default offload flows including jump and default RSS for
 * traffic.
 *
 * @param cfg
 * Port configuration, see doca_flow_cfg for details.
 * @param error
 * Output error, set doca_flow_error for details.
 * @return
 * Port handler on success, NULL otherwise and error is set.
 */
__DOCA_EXPERIMENTAL
struct doca_flow_port *
doca_flow_port_start(const struct doca_flow_port_cfg *cfg,
		     struct doca_flow_error *error);

/**
 * @brief Stop a doca port.
 *
 * Stop the port, disable the traffic.
 *
 * @param port
 * Port struct.
 * @return
 * 0 on success, negative on failure.
 */
__DOCA_EXPERIMENTAL
int
doca_flow_port_stop(struct doca_flow_port *port);

/**
 * @brief Get pointer of user private data.
 *
 * User can manage specific data structure in port structure.
 * The size of the data structure is given on port configuration.
 * See doca_flow_cfg for more details.
 *
 * @param port
 * Port struct.
 * @return
 * Private data head pointer.
 */
__DOCA_EXPERIMENTAL
uint8_t*
doca_flow_port_priv_data(struct doca_flow_port *port);

/**
 * @brief Create one new pipe.
 *
 * Create new pipeline to match and offload specific packets, the pipe
 * configuration includes the following components:
 *
 *     match: Match one packet by inner or outer fields.
 *     match_mask: The mask for the matched items.
 *     actions: Includes the modify specific packets fields, Encap and
 *                  Decap actions.
 *     monitor: Includes Count, Age, and Meter actions.
 *     fwd: The destination of the matched action, include RSS, Hairpin,
 *             Port, and Drop actions.
 *
 * This API will create the pipe, but would not start the HW offload.
 *
 * @param cfg
 * Pipe configuration.
 * @param fwd
 * Fwd configuration for the pipe.
 * @param fwd_miss
 * Fwd_miss configuration for the pipe. NULL for no fwd_miss.
 * When creating a pipe if there is a miss and fwd_miss configured,
 * packet steering should jump to it.
 * @param error
 * Output error, set doca_flow_error for details.
 * @return
 * Pipe handler on success, NULL otherwise and error is set.
 */
__DOCA_EXPERIMENTAL
struct doca_flow_pipe *
doca_flow_create_pipe(const struct doca_flow_pipe_cfg *cfg,
		const struct doca_flow_fwd *fwd,
		const struct doca_flow_fwd *fwd_miss,
		struct doca_flow_error *error);

/**
 * @brief Create control pipe.
 *
 * Control pipe is a special type of pipe that can have dynamic
 * matches and forward with priority. Number of entries is limited (<64).
 *
 * @param port
 * Port struct.
 * @param error
 * Output error, set doca_flow_error for details.
 * @return
 * pipe handler or NULL on failure.
 */
struct doca_flow_pipe *
doca_flow_create_control_pipe(struct doca_flow_port *port,
			struct doca_flow_error *error);


/**
 * @brief Add one new entry to a pipe.
 *
 * When a packet matches a single pipe, will start HW offload. The pipe only
 * defines which fields to match. When offloading, we need detailed information
 * from packets, or we need to set some specific actions that the pipe did not
 * define. The parameters include:
 *
 *    match: The packet detail fields according to the pipe definition.
 *    actions: The real actions according to the pipe definition.
 *    monitor: Defines the monitor actions if the pipe did not define it.
 *    fwd: Define the forward action if the pipe did not define it.
 *
 * This API will do the actual HW offload, with the information from the fields
 * of the input packets.
 *
 * @param pipe_queue
 * Queue identifier.
 * @param pipe
 * Pointer to pipe.
 * @param match
 * Pointer to match, indicate specific packet match information.
 * @param actions
 * Pointer to modify actions, indicate specific modify information.
 * @param monitor
 * Pointer to monitor actions.
 * @param fwd
 * Pointer to fwd actions.
 * @param error
 * Output error, set doca_flow_error for details.
 * @return
 * Pipe entry handler on success, NULL otherwise and error is set.
 */
__DOCA_EXPERIMENTAL
struct doca_flow_pipe_entry*
doca_flow_pipe_add_entry(uint16_t pipe_queue,
			 struct doca_flow_pipe *pipe,
			 const struct doca_flow_match *match,
			 const struct doca_flow_actions *actions,
			 const struct doca_flow_monitor *monitor,
			 const struct doca_flow_fwd *fwd,
			 struct doca_flow_error *error);


/**
 * @brief Add one new entry to a control pipe.
 *
 * Refer to doca_flow_pipe_add_entry.
 *
 * @param pipe_queue
 * Queue identifier.
 * @param priority
 * Priority value.
 * @param pipe
 * Pointer to pipe.
 * @param match
 * Pointer to match, indicate specific packet match information.
 * @param match_mask
 * Pointer to match mask information.
 * @param fwd
 * Pointer to fwd actions.
 * @param error
 * Output error, set doca_flow_error for details.
 * @return
 * Pipe entry handler on success, NULL otherwise and error is set.
 */
__DOCA_EXPERIMENTAL
struct doca_flow_pipe_entry*
doca_flow_control_pipe_add_entry(uint16_t pipe_queue,
			uint8_t priority,
			struct doca_flow_pipe *pipe,
			const struct doca_flow_match *match,
			const struct doca_flow_match *match_mask,
			const struct doca_flow_fwd *fwd,
			struct doca_flow_error *error);

/**
 * @brief Free one pipe entry.
 *
 * This API will free the pipe entry and cancel HW offload. The
 * Application receives the entry pointer upon creation and if can
 * call this function when there is no more need for this offload.
 * For example, if the entry aged, use this API to free it.
 *
 * @param pipe_queue
 * Queue identifier.
 * @param entry
 * The pipe entry to be removed.
 * @return
 * 0 on success, negative on failure.
 */
__DOCA_EXPERIMENTAL
int
doca_flow_pipe_rm_entry(uint16_t pipe_queue,
			struct doca_flow_pipe_entry *entry);

/**
 * @brief Destroy one pipe
 *
 * Destroy the pipe, and the pipe entries that match this pipe.
 *
 * @param port_id
 * Port id of the port.
 * @param pipe
 * Pointer to pipe.
 */
__DOCA_EXPERIMENTAL
void
doca_flow_destroy_pipe(uint16_t port_id,
		       struct doca_flow_pipe *pipe);

/**
 * @brief Flush pipes of one port
 *
 * Destroy all pipes and all pipe entries belonging to the port.
 *
 * @param port_id
 * Port id of the port.
 */
__DOCA_EXPERIMENTAL
void
doca_flow_flush_pipe(uint16_t port_id);

/**
 * @brief Destroy a doca port.
 *
 * Destroy the doca port, free all resources of the port.
 *
 * @param port_id
 * Port id of the port.
 */
__DOCA_EXPERIMENTAL
void
doca_flow_destroy_port(uint16_t port_id);

/**
 * @brief Dump pipe of one port
 *
 * Dump all pipes and all entries information belong to this port.
 *
 * @param port_id
 * Port id of the port.
 * @param f
 * The output file of the pipe information.
 */
__DOCA_EXPERIMENTAL
void
doca_flow_dump_pipe(uint16_t port_id, FILE *f);

/**
 * @brief Extract information about specific entry
 *
 * Query the packet statistics about specific pipe entry
 *
 * @param entry
 * The pipe entry toe query.
 * @param query_stats
 * Data retrieved by the query.
 * @return
 * 0 on success, negative on failure.
 */
__DOCA_EXPERIMENTAL
int
doca_flow_query(struct doca_flow_pipe_entry *entry,
		struct doca_flow_query *query_stats);

/**
 * @brief Handle aging of flows in queue.
 *
 * Go over all flows and release aged flows from being
 * tracked. The entries array will be filled with aged flows.
 *
 * Since the number of flows can be very large, it can take
 * a significant amount of time to go over all flows so this
 * function is limited by time quota, which means it might
 * return without handling all flows which requires the user
 * to call it again. Once a full cycle is done this function will
 * return -1.
 *
 * @param queue
 * Queue identifier.
 * @param quota
 * Max time quota in micro seconds for this function to handle aging.
 * @param entries
 * User input entries array for the aged flows.
 * @param len
 * User input length of entries array.
 * @return
 * > 0 the number of aged flows filled in entries array.
 * 0 no aged entries in current call.
 * -1 full cycle done.
 */
__DOCA_EXPERIMENTAL
int
doca_flow_handle_aging(uint16_t queue, uint64_t quota,
		       struct doca_flow_aged_query *entries, int len);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* FLOW_H_ */
