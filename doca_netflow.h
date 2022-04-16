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
 * @file doca_netflow.h
 * @page netflow
 * @defgroup NETFLOW NetFlow
 *
 * DOCA lib for exporting a netflow packet to a netflow collector.
 *
 * This lib simplifies and centralizes the formatting and exporting of netflow packets.
 * Netflow is a protocol for exporting information about the device network flows
 * to a netflow collector that will aggregate and analyze the data.
 * After creating the conf file and invoking the init function, the lib's send function
 * can be called with netflow struct to send a netflow packet with the format
 * to the collector of choice, as specified in the conf file.
 * The lib uses the netflow protocol specified by cisco.
 * @see https://netflow.caligare.com/netflow_v9.htm
 *
 * Conf File structure:
 *
 * doca_netflow.conf
 *
 * [doca_netflow_conf]
 *
 * target = <hostname = name/ipv4/ipv6>:<port = integer>
 *
 * source_id = <ID = integer>
 *
 * version = <version = 9>
 *
 *
 * doca_netflow_default.conf
 *
 * [doca_netflow_conf]
 *
 * target = 127.0.0.1:2055
 *
 * source_id = 10
 *
 * version  = 9
 *
 *
 * Limitations:
 *
 * The lib supports the netflow V9 format. The lib is not thread safe.
 *
 * @{
 */

#ifndef _DOCA_NETFLOW__H_
#define _DOCA_NETFLOW__H_

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "doca_compat.h"
#include "doca_netflow_types.h"

/**
 * @brief default conf path to look for
 */
#define DOCA_NETFLOW_CONF_DEFAULT_PATH "/etc/doca_netflow.conf"

/**
 * @brief Flow record, represent a flow at specific moment, usually after
 * a flow ends or after some timeout. Each one is a data record that will appear
 * in the collector. This template is based on V5 fields with additional V9 fields.
 * @note all fields are in network byte order.
 */
struct doca_netflow_default_record {
	__be32 src_addr_v4;				/**< Source IPV4 Address */
	__be32 dst_addr_v4;				/**< Destination IPV4 Address */
	struct in6_addr src_addr_v6;			/**< Source IPV6 Address */
	struct in6_addr dst_addr_v6;			/**< Destination IPV6 Address */
	__be32 next_hop_v4;				/**< Next hop router's IPV4 Address */
	struct in6_addr next_hop_v6;			/**< Next hop router's IPV6 Address */
	__be16 input;					/**< Input interface index */
	__be16 output;					/**< Output interface index */
	__be16 src_port;				/**< TCP/UDP source port number or equivalent */
	__be16 dst_port;				/**< TCP/UDP destination port number or equivalent */
	uint8_t tcp_flags;				/**< Cumulative OR of tcp flags */
	uint8_t protocol;				/**< IP protocol type (for example, TCP = 6;UDP = 17) */
	uint8_t tos;					/**< IP Type-of-Service */
	__be16 src_as;					/**< originating AS of source address */
	__be16 dst_as;					/**< originating AS of destination address */
	uint8_t src_mask;				/**< source address prefix mask bits */
	uint8_t dst_mask;				/**< destination address prefix mask bits */
	__be32 d_pkts;					/**< Packets sent in Duration */
	__be32 d_octets;				/**< Octets sent in Duration. */
	__be32 first;					/**< SysUptime at start of flow */
	__be32 last;					/**< and of last packet of flow */
	__be64 flow_id;					/**< This identifies a transaction within a connection */
	char application_name[DOCA_NETFLOW_APPLICATION_NAME_DEFAULT_LENGTH];
	/**< Name associated with a classification*/
} __attribute__((packed));

/**
 * @brief One field in netflow template, please look at doca_netflow_types for type macros
 */
struct doca_netflow_flowset_field {
	int type;   /**< field number id (see link) - will be converted to uint16 */
	int length; /**< field len in bytes (see link) - will be converted to uint16 */
};

/**
 * @brief Template for the records.
 * struct record_example {
 *	uint32_t src_addr_V4;
 *	uint32_t dst_addr_V4;
 * }
 * struct doca_netflow_flowset_field fields[] = {
 *	{.type = DOCA_NETFLOW_IPV4_SRC_ADDR, .length = DOCA_NETFLOW_IPV4_SRC_ADDR_DEFAULT_LENGTH},
 *	{.type = DOCA_NETFLOW_IPV4_DST_ADDR, .length = DOCA_NETFLOW_IPV4_DST_ADDR_DEFAULT_LENGTH}
 * };
 * struct doca_netflow_template template = {
 *	.field_count = 2;
 *	.fields = fields;
 * };
 * @note all fields are in network byte order.
 */
struct doca_netflow_template {
	int field_count; /**< number of fields in 'fields' array - will be converted to uint16 */
	struct doca_netflow_flowset_field *fields; /**< array of field info */
};

/**
 * @brief Return a default doca_netflow_template for use in send function,
 * if using default template use doca_netflow_default_record struct for records.
 *
 * @return
 *   pointer containing the default template
 */
__DOCA_EXPERIMENTAL
const struct doca_netflow_template *doca_netflow_template_default_get(void);

/**
 * @brief Init exporter memory, set configs and open connection.
 *
 * @param netflow_conf_file
 *   Doca netflow configuration file pointer including a section marked as
 *   [doca_netflow_conf], if a NULL pointer is given will use the default
 *   path, as defined by DOCA_NETFLOW_CONF_DEFAULT_PATH.
 *   This function can be called again only after doca_netflow_exporter_destroy was called.
 * @return
 *   0 on success, error code otherwise.
 */
__DOCA_EXPERIMENTAL
int doca_netflow_exporter_init(const char *netflow_conf_file);

/**
 * @brief Sending netflow records. Need to init first.
 * @param template
 *   Template pointer for how the records are structured.
 *   for more info reffer to doca_netflow_template.
 * @param records
 *   Array of pointers to the flows structs to send, must be packed.
 *   strings must be a direct array in the struct not a pointer.
 * @param length
 *   Records array size.
 * @param error
 *   If return value is -1 populate this field with the error.
 * @return
 *   Number of records sent, -1 on error.
 * @note if the return value is positive but not equal to length then
 * just some of the records were sent. The send function should run
 * again with the remaining records. Please reffer to the example.
 * @note When sending more then 30 records the lib splits the records
 * to multiple packets because a single packet can only send up to 30 records
 * (Netflow protocol limit)
 */
__DOCA_EXPERIMENTAL
int doca_netflow_exporter_send(const struct doca_netflow_template *netflow_template, const void **records,
			       size_t length, int *error);

/**
 * @brief Free the exporter memory and close the connection.
 */
__DOCA_EXPERIMENTAL
void doca_netflow_exporter_destroy(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
