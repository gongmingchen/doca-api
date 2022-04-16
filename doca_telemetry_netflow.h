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
 * @file doca_telemetry_netflow.h
 * @page telemetry_netflow
 * @defgroup TELEMETRY NetFlow
 *
 * DOCA lib for exporting a netflow packet to a netflow collector through the
 * telemetry service.
 *
 * This lib simplifies and centralizes the formatting and exporting of netflow packets.
 * Netflow is a protocol for exporting information about the device network flows
 * to a netflow collector that will aggregate and analyze the data.
 * After creating conf file and invoke init function, the lib send function
 * can be called with netflow struct to send a netflow packet with the format
 * to the collector of choice specified in the conf file.
 * The lib uses the netflow protocol specified by cisco.
 * @see https://netflow.caligare.com/netflow_v9.htm
 *
 * Limitations:
 *
 * The lib supports the netflow V9 format. The lib is not thread safe.
 *
 * @{
 */

#ifndef _DOCA_TELEMETRY_NETFLOW__H_
#define _DOCA_TELEMETRY_NETFLOW__H_

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "doca_telemetry_netflow_types.h"
#include "doca_telemetry.h"

/**
 * @brief NetFlow Application ID
 *
 * @note This GUID cannot change
 */
/* 9910c128-3961-47e6-be6c-715a0f03add6 */
#define DOCA_NETFLOW_APP_ID                            \
	{                                                  \
		0x99, 0x10, 0xc1, 0x28, 0x39, 0x61, 0x47, 0xe6,\
		0xbe, 0x6c, 0x71, 0x5a, 0x0f, 0x03, 0xad, 0xd6 \
	}

/**
 * @brief One field in netflow template, please look at doca_telemetry_netflow_types for type macros
 */
struct doca_telemetry_netflow_flowset_field {
	uint16_t type;   /**< field number id (see link) */
	uint16_t length; /**< field len in bytes (see link) */
};

/**
 * @brief Template for the records.
 * struct record_example {
 *	uint32_t src_addr_V4;
 *	uint32_t dst_addr_V4;
 * }
 * struct doca_telemetry_netflow_flowset_field fields[] = {
 *	{.type = DOCA_NETFLOW_IPV4_SRC_ADDR, .length = DOCA_NETFLOW_IPV4_SRC_ADDR_DEFAULT_LENGTH},
 *	{.type = DOCA_NETFLOW_IPV4_DST_ADDR, .length = DOCA_NETFLOW_IPV4_DST_ADDR_DEFAULT_LENGTH}
 * };
 * struct doca_telemetry_netflow_template netflow_template = {
 *	.field_count = 2;
 *	.fields = fields;
 * };
 * @note all fields are in network byte order.
 */
struct doca_telemetry_netflow_template {
	uint16_t                                     field_count;
	/**< number of fields in 'fields' array */
	struct doca_telemetry_netflow_flowset_field *fields;
	/**< array of field info */
};

/**
 * @brief DOCA netflow sending attribute. Applied to all DOCA sources.
 *
 * Use to enable/disable netflow sending to a collector. Disabled by the defalt.
 */
struct doca_telemetry_netflow_send_attr_t {
	char *netflow_collector_addr;
	/**< User defined netflow collector's IP address */
	uint16_t netflow_collector_port;
	/**< User defined netflow collector's port */
};

/**
 * @brief Init exporter memory, set configs and open connection.
 *
 * The Source ID field is a 32-bit value that is used to guarantee uniqueness for
 * all flows exported from a particular device (see link).
 *
 * This function can be called again only after doca_telemetry_netflow_destroy was
 * called.
 *
 * @param source_id
 * Unique source ID.
 * @return
 * 0 on success, a negative telemetry_status on error.
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_netflow_init(uint16_t source_id);


/**
 * @brief Set buffer attributes to DOCA netflow.
 *
 * @param buffer_attr
 * Attributes to set.
 * @return
 * 0 on success, a negative telemetry_status on error.
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_netflow_buffer_attr_set(struct doca_telemetry_buffer_attr_t *buffer_attr);


/**
 * @brief Set file write attributes to DOCA netflow.
 *
 * @param file_attr
 * Attributes to set.
 * @return
 * 0 on success, a negative telemetry_status on error.
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_netflow_file_write_attr_set(struct doca_telemetry_file_write_attr_t *file_attr);

/**
 * @brief Set IPC tarnsport attributes to DOCA netflow.
 *
 * @param ipc_attr
 * Attribute to set.
 * @return
 * 0 on success, a negative telemetry_status on error.
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_netflow_ipc_attr_set(struct doca_telemetry_ipc_attr_t *ipc_attr);


/**
 * @brief Set send attributes to DOCA netflow.
 *
 * @param netflow_send_attr
 * Attribute to set.
 * @return
 * 0 on success, a negative telemetry_status on error.
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_netflow_send_attr_set(
	struct doca_telemetry_netflow_send_attr_t *netflow_send_attr);


/**
 * @brief Finalizes netflow setup.
 *
 * Do NOT set attributes after this call.
 *
 * @param source_attr
 * Source attributes.
 * @return
 * 0 on success, a negative telemetry_status on error.
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_netflow_start(struct doca_telemetry_source_name_attr_t *source_attr);

/**
 * @brief Sending netflow records. Need to init first.
 *
 * @param template
 *   Template pointer of how the records are structured.
 *   For more info reffer to doca_telemetry_netflow_template.
 * @param records
 *   Array of pointers to the flows structs to send, must be packed.
 *   Strings must be an array in the struct, not a pointer.
 * @param nof_records
 *   Records array size.
 * @param nof_records_sent
 *   If not NULL, it will be filled with amount of records sent.
 * @return
 *   0 on success, a negative telemetry_status on error.
 * @note When sending more then 30 records the lib splits the records
 * to multiple packets because each packet can only send up to 30 records
 * (Netflow protocol limit)
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_netflow_send(const struct doca_telemetry_netflow_template *netflow_template,
			const void **records, size_t nof_records, size_t *nof_records_sent);

/**
 * @brief Free the exporter memory and close the connection.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_netflow_destroy(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _DOCA_TELEMETRY_NETFLOW__H_ */
