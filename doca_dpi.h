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
 * @file doca_dpi.h
 * @page doca dpi
 * @defgroup DPI Deep packet inspection
 * DOCA Deep packet inspection library. For more details please refer to the
 * user guide on DOCA devzone.
 *
 * @{
 */

#ifndef DOCA_DPI_H_
#define DOCA_DPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <linux/types.h>

#include <rte_mbuf.h>

#include <doca_compat.h>

/**
 * @brief Status of enqueue operation
 */
enum doca_dpi_enqueue_status_t {
	DOCA_DPI_ENQ_PROCESSING,
	/**< Packet enqueued for processing */
	DOCA_DPI_ENQ_PACKET_EMPTY,
	/**< No payload, packet was not queued */
	DOCA_DPI_ENQ_BUSY,
	/**< Packet cannot be enqueued, queue is full */
	DOCA_DPI_ENQ_INVALID_DB,
	/**< load_signatures failed, or was never called */
	DOCA_DPI_ENQ_INTERNAL_ERR,
	/* Other system errors possible */
};

/**
 * @brief Status of dequeue operation
 */
enum doca_dpi_dequeue_status_t {
	DOCA_DPI_DEQ_NA,
	/**< No DPI enqueued jobs done, or no packets to dequeue */
	DOCA_DPI_DEQ_READY,
	/**< DPI Job and result is valid */
};

/**
 * @brief Status of enqueued entry
 */
enum doca_dpi_flow_status_t {
	DOCA_DPI_STATUS_LAST_PACKET = 1 << 1,
	/**< Indicates there are no more packets in queue from this flow. */
	DOCA_DPI_STATUS_DESTROYED = 1 << 2,
	/**< Indicates flow was destroyed while being processed */
	DOCA_DPI_STATUS_NEW_MATCH = 1 << 3,
	/**< Indicates flow was matched on current dequeue */
};

/**
 * @brief Signature action. Some signatures may come with an action.
 */
enum doca_dpi_sig_action_t {
	DOCA_DPI_SIG_ACTION_NA,
	/**< Action not available for signature */
	DOCA_DPI_SIG_ACTION_ALERT,
	/**< Alert */
	DOCA_DPI_SIG_ACTION_PASS,
	/**< Signature indicates that the flow is allowed */
	DOCA_DPI_SIG_ACTION_DROP,
	/**< Signature indicates that the flow should be dropped */
	DOCA_DPI_SIG_ACTION_REJECT,
	/**< Send RST/ICMP unreach error to the sender of the matching packet */
	DOCA_DPI_SIG_ACTION_REJECTSRC,
	/**< Send RST/ICMP unreach error to the sender of the matching packet */
	DOCA_DPI_SIG_ACTION_REJECTDST,
	/**< Send RST/ICMP error packet to receiver of the matching packet */
	DOCA_DPI_SIG_ACTION_REJECTBOTH,
	/**< Send RST/ICMP error packets to both sides of the conversation */
};

/**
 * @brief DPI init configuration
 */
struct doca_dpi_config_t {
	uint16_t nb_queues;
	/**< Number of DPI queues */
	uint32_t max_packets_per_queue;
	/**< Number of packets concurrently processed by the DPI engine. */
	uint32_t max_sig_match_len;
	/**< The minimum required overlap between two packets for regex match */
};

/**
 * @brief L2-L4 flow information
 */
struct doca_dpi_parsing_info {
	__be16 ethertype;
	/**< Ethertype of the packet in network byte order*/
	uint8_t l4_protocol;
	/**< Layer 4 protocol */
	in_port_t l4_dport;
	/**< Layer 4 destination port in network byte order*/
	in_port_t l4_sport;
	/**< Layer 4 source port in network byte order*/
	union {
		struct in_addr ipv4;
		/**< Ipv4 destination address in network byte order */
		struct in6_addr ipv6;
		/**< Ipv6 destination address in network byte order */
	} dst_ip;
	/**< IP destination address */
	union {
		struct in_addr ipv4;
		/**< Ipv4 source address in network byte order */
		struct in6_addr ipv6;
		/**< Ipv6 source address in network byte order */
	} src_ip;
	/**< IP source address */
};

/**
 * @brief Signature info
 */
struct doca_dpi_sig_info {
	uint32_t sig_id;
	/**< Signature ID as provided in the signature */
	int action;
	/**< The action as provided in the signature */
};

/**
 * @brief Extra signature data
 */
struct doca_dpi_sig_data {
	uint32_t sig_id;
	/**< Signature ID as provided in the signature */
	char name[1024];
	/**< Signature name */
};

/**
 * @brief Dequeue result
 */
struct doca_dpi_result {
	bool matched;
	/**< Indicates flow was matched */
	void *user_data;
	/**< User data provided on enqueue */
	struct rte_mbuf *pkt;
	/**< Pkt provided on enqueue */
	struct doca_dpi_sig_info info;
	/**< Signature information */
	int status_flags;
	/**< doca_dpi_flow_status flags */
};

/**
 * @brief DPI statistics
 */
struct doca_dpi_stat_info {
	uint32_t nb_scanned_pkts;
	/**< Total number of scanned packets */
	uint32_t nb_matches;
	/**< Total number of signature matches */
	uint32_t nb_http_parser_based;
	/**< Total number of http signature matches */
	uint32_t nb_ssl_parser_based;
	/**< Total number of ssl signature matches */
	uint32_t nb_tcp_based;
	/**< Total number of tcp signature matches */
	uint32_t nb_udp_based;
	/**< Total number of udp signature matches */
	uint32_t nb_other_l4;
	/**< Total number of other l4 signature matches */
	uint32_t nb_other_l7;
	/**< Total number of other l7 signature matches */
};

/**
 * @brief Opaque flow context
 */
struct doca_dpi_flow_ctx;

/**
 * @brief Opaque DPI context
 */
struct doca_dpi_ctx;

/**
 * @brief Initialize the DPI library.
 *
 * This function must be invoked first before any function in the API.
 * It should be invoked once per process.
 * This call will probe the first regex device it finds (0).
 * @param config
 *   See doca_dpi_config_t for details.
 * @param error
 *   Output error, negative value indicates an error.
 * @return
 *   doca_dpi_ctx - dpi opaque context, NULL on error.
 */
__DOCA_EXPERIMENTAL
struct doca_dpi_ctx *doca_dpi_init(const struct doca_dpi_config_t *config, int *error);

/**
 * @brief Free the DPI memory and releases the regex engine.
 *
 * @param ctx
 *   DPI context to destroy.
 */
__DOCA_EXPERIMENTAL
void doca_dpi_destroy(struct doca_dpi_ctx *ctx);

/**
 * @brief Loads the cdo file.
 *
 * The cdo file contains signature information. The cdo file must be
 * loaded before any enqueue call.
 *
 * Database update:
 * When a new signatures database is available, the user may call this
 * function again.
 * The newly loaded CDO must contain the signatures of the previously loaded CDO
 * or result will be undefined.
 * @param ctx
 *   The DPI context.
 * @param cdo_file
 *   CDO file created by the DPI compiler.
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_dpi_load_signatures(struct doca_dpi_ctx *ctx, const char *cdo_file);

/**
 * @brief Enqueue a new DPI job for processing.
 *
 * This function is thread-safe per queue. For best performance it should
 * always be called from the same thread/queue on which the flow was created. See
 * Multithreading section of the DPI Programming Guide for more details.
 *
 * Once a packet is enqueued, user must not change, reuse or free the
 * mbuf while it is being processed.
 * See "Packet Ownership" section of the DPI Programming Guide for more details.
 *
 * The injected packet has to be stripped of FCS.
 * A packet will not be enqueued if:
 * - Payload length = 0
 *
 * @param flow_ctx
 * The flow context handler.
 * @param pkt
 * The mbuf to be processed.
 * @param initiator
 * Indicates to which direction the packet belongs.
 * 1 - if the packet arrives from client to server.
 * 0 - if the packet arrives from server to client.
 * Typically, the first packet will arrive from the initiator (client).
 * @param payload_offset
 * Indicates where the packet's payload begins.
 * @param user_data
 * Private user data to b returned when the DPI job is dequeued.
 * @return
 * doca_dpi_enqueue_status_t or other error code.
 */
__DOCA_EXPERIMENTAL
int doca_dpi_enqueue(struct doca_dpi_flow_ctx *flow_ctx, struct rte_mbuf *pkt,
		     bool initiator, uint32_t payload_offset, void *user_data);

/**
 * @brief Dequeues packets after processing.
 *
 * Only packets enqueued for processing will be returned by this API. Packets
 * will return in the order they were enqueued.
 *
 * @param ctx
 * The DPI context.
 * @param dpi_q
 * The DPI queue from which to dequeue the flows' packets.
 * @param result
 * Output, matching result.
 * @return
 * doca_dpi_dequeue_status_t if successful, error code otherwise
 */
__DOCA_EXPERIMENTAL
int doca_dpi_dequeue(struct doca_dpi_ctx *ctx, uint16_t dpi_q, struct doca_dpi_result *result);

/**
 * @brief Creates a new flow on a queue
 *
 * Must be called before enqueuing any new packet.
 * A flow must not be created on 2 different queues.
 *
 * @param ctx
 * The DPI context.
 * @param dpi_q
 * The DPI queue on which to create the flows
 * @param parsing_info
 * L3/L4 information.
 * @param error
 * Output, Negative if error occurred.
 * @param result
 * Output, If flow was matched based on the parsing info, result->matched will be true.
 * @return
 * NULL on error.
 */
__DOCA_EXPERIMENTAL
struct doca_dpi_flow_ctx *doca_dpi_flow_create(struct doca_dpi_ctx *ctx, uint16_t dpi_q,
					       const struct doca_dpi_parsing_info *parsing_info,
					       int *error,
					       struct doca_dpi_result *result);

/**
 * @brief Destroys a flow on a queue
 *
 * Should be called when a flow is terminated or times out
 *
 * @param flow_ctx
 * The flow context to destroy.
 */
__DOCA_EXPERIMENTAL
void doca_dpi_flow_destroy(struct doca_dpi_flow_ctx *flow_ctx);

/**
 * @brief Query a flow's match
 *
 * @param flow_ctx
 * The flow context of the flow to be queried.
 * @param result
 * Output, latest match on this flow.
 * Only "matched" and "info" fields in the result parameter are valid.
 * @return
 * 0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_dpi_flow_match_get(const struct doca_dpi_flow_ctx *flow_ctx,
			    struct doca_dpi_result *result);

/**
 * @brief Returns a specific sig info
 *
 * @param ctx
 * The DPI context.
 * @param sig_id
 * The signature ID.
 * @param sig_data
 * Output of the sig metadata.
 * @return
 * 0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_dpi_signature_get(const struct doca_dpi_ctx *ctx, uint32_t sig_id,
			   struct doca_dpi_sig_data *sig_data);

/**
 * @brief Returns all signatures
 *
 * It is the responsibility of the user to free the array.
 * Because this function copies all the sig info, it is highly recommended
 * to call this function only once after loading the database, and not during
 * packet processing.
 *
 * @param ctx
 * The DPI context.
 * @param sig_data
 * Output of the sig data.
 * @return
 * Number of signatures on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_dpi_signatures_get(const struct doca_dpi_ctx *ctx, struct doca_dpi_sig_data **sig_data);

/**
 * @brief Returns DPI statistics.
 *
 * @param ctx
 * The DPI context.
 * @param clear
 * Clear the statistics after fetching them.
 * @param stats
 * Output struct containing the statistics.
 */
__DOCA_EXPERIMENTAL
void doca_dpi_stat_get(const struct doca_dpi_ctx *ctx, bool clear,
		       struct doca_dpi_stat_info *stats);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DPI_H_ */
