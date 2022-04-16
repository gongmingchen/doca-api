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
 * @file doca_telemetry.h
 * @page telemetry
 * @defgroup TELEMETRY Telemetry Service Library
 *
 * DOCA lib for exporting events to the telemetry service.
 *
 * @{
 */

#ifndef DOCA_TELEMETRY_H_
#define  DOCA_TELEMETRY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <doca_compat.h>

enum telemetry_status {
	DOCA_TELEMETRY_OK = 0,
	DOCA_TELEMETRY_ERROR = 1,
	DOCA_TELEMETRY_ALLOC_ERROR,
	DOCA_TELEMETRY_CLX_CONTEXT_INIT_ERROR,
	DOCA_TELEMETRY_CLX_CONTEXT_CLONE_ERROR,
	DOCA_TELEMETRY_SOURCE_ATTR_NOT_SET,
	DOCA_TELEMETRY_INTERNAL_BUFFER_ERROR,
	DOCA_TELEMETRY_BAD_STATE_ERROR,
	DOCA_TELEMETRY_BAD_PARAM_ERROR,
};

/**
 * @brief DOCA schema field type index.
 */
typedef uint8_t doca_telemetry_type_index_t;

/**
 * @brief DOCA schema type index type.
 */
typedef uint64_t doca_telemetry_timestamp_t;

/**
 * @brief DOCA schema field.
 */
typedef struct doca_telemetry_field_info {
	const char *field_name;
	/**< Name of field */
	const char *description;
	/**< Field descripion */
	const char *type_name;
	/**< Name of type that is already in schema (including built-in types). */
	uint16_t    array_length;
	/**< Array length for this field type. Set to: 1 to register single value
	 *   or >1 to register array of values.
	 */
} doca_telemetry_field_info_t;

/**
 * @brief NUM_OF_DOCA_FIELDS is macro for fast counting number
 *        of fields in user-defined fields array.
 */
#define NUM_OF_DOCA_FIELDS(type) (sizeof(type)/sizeof(doca_telemetry_field_info_t))


/**
 * @brief DOCA_TELEMETRY_FIELD_TYPE_{} are data types that
 *        are used to create doca_telemetry_field_info_t;
 */
#define DOCA_TELEMETRY_FIELD_TYPE_BOOL          "bool"
#define DOCA_TELEMETRY_FIELD_TYPE_CHAR          "char"
#define DOCA_TELEMETRY_FIELD_TYPE_SHORT         "short"
#define DOCA_TELEMETRY_FIELD_TYPE_IN            "int"
#define DOCA_TELEMETRY_FIELD_TYPE_LONG          "long"
#define DOCA_TELEMETRY_FIELD_TYPE_LONGLONG      "long long"
#define DOCA_TELEMETRY_FIELD_TYPE_UCHAR         "unsigned char"
#define DOCA_TELEMETRY_FIELD_TYPE_USHORT        "unsigned short"
#define DOCA_TELEMETRY_FIELD_TYPE_UINT          "unsigned int"
#define DOCA_TELEMETRY_FIELD_TYPE_ULONG         "unsigned long"
#define DOCA_TELEMETRY_FIELD_TYPE_ULONGLONG     "long long"
#define DOCA_TELEMETRY_FIELD_TYPE_FLOAT         "float"
#define DOCA_TELEMETRY_FIELD_TYPE_DOUBLE        "double"
#define DOCA_TELEMETRY_FIELD_TYPE_INT8          "int8_t"
#define DOCA_TELEMETRY_FIELD_TYPE_INT16         "int16_t"
#define DOCA_TELEMETRY_FIELD_TYPE_INT32         "int32_t"
#define DOCA_TELEMETRY_FIELD_TYPE_INT64         "int64_t"
#define DOCA_TELEMETRY_FIELD_TYPE_UINT8         "uint8_t"
#define DOCA_TELEMETRY_FIELD_TYPE_UINT16        "uint16_t"
#define DOCA_TELEMETRY_FIELD_TYPE_UINT32        "uint32_t"
#define DOCA_TELEMETRY_FIELD_TYPE_UINT64        "uint64_t"
#define DOCA_TELEMETRY_FIELD_TYPE_TIMESTAMP     DOCA_TELEMETRY_FIELD_TYPE_UINT64

#define DOCA_GUID_SIZE 16
/**
 * @brief DOCA GUID type
 */
typedef uint8_t doca_guid_t[DOCA_GUID_SIZE];

/**
 * @brief DOCA telemetry source attributes: id and tag.
 *
 * ID and Tag are used to create proper folder structure. All the data collected from
 * the same host is written to "source_id" folder under data root. Binary file will
 * have {source_tag}_{timestamp}.bin name format.
 */
struct doca_telemetry_source_name_attr_t {
	char *source_id;
	/**< Hostname or guid. */
	char *source_tag;
	/**< User defined datafile name prefix. */
};


/**
 * @brief DOCA schema buffer attribute. Applied to all DOCA sources.
 *
 * Use to set internal buffer_size. All DOCA sources will have buffers of the same size.
 * The buffer is flushed once it is full, or upon invocation of doca_telemetry_source_flush().
 * The buffer size is set to 60,000 by default.
 * data_root is the data folder for storing the data and data schema_{hash}.json files.
 */
struct doca_telemetry_buffer_attr_t {
	uint64_t  buffer_size;
	/**< Size of the internal buffer. */
	char     *data_root;
	/**< Path for where the data and schema will be stored. */
};


/**
 * @brief DOCA schema file write attribute. Applied to all DOCA sources.
 *
 * Use to enable/disable file write onto storage under data_root.
 * File write is disabled by default.
 */
struct doca_telemetry_file_write_attr_t {
	bool                        file_write_enabled;
	/**< User defined switch for enabling/disabling local file write.
	 *   Disabled by the default.
	 */
	size_t                      max_file_size;
	/**< Maximum size of binary data file. Once this size is reached,
	 *   a new binary file will be created.
	 */
	doca_telemetry_timestamp_t  max_file_age;
	/**< Maximum file age. Once current file is older than this threshold
	 *   a new file will be created.
	 */
};


/**
 * @brief DOCA schema file write attribute. Applied to all DOCA sources.
 *
 * Use to enable/disable ipc transport. Enabled by default.
 * Default ipc_sockets_dir is '/opt/mellanox/doca/services/telemetry/ipc_sockets'
 */
struct doca_telemetry_ipc_attr_t {
	bool  ipc_enabled;
	/**< User defined switch for enabling/disabling IPC transport. */
	char *ipc_sockets_dir;
	/**< Path to a folder containing Telemetry Service sockets. */
};


/**
 * @brief DOCA schema opaque events attribute. Applied to all DOCA sources.
 *
 * Use to enable/disable opaque events transport. Disabled by default.
 */
struct doca_telemetry_opaque_events_attr_t {
	bool opaque_events_enabled;
	/**< User defined switch for enabling/disabling Opaque Events sending. */
};


/**
 * @brief DOCA schema IPC attribute. Applied to all DOCA sources.
 *
 * Used to overwrite default values of timeouts for attach/reattach attempts and
 * IPC socket timeout.
 */
struct doca_telemetry_ipc_timeout_attr_t {
	uint32_t ipc_max_reconnect_time_msec;
	/**< Time limit for reconnect attempts. If the limit is reached, the client
	 *   is considered disconnected. Default is 100 msec.
	 */
	int      ipc_max_reconnect_tries;
	/**< Number of reconnect attempts during reconnection period. Default is 3. */
	uint32_t ipc_socket_timeout_msec;
	/**< Timeout for IPC messaging socket. If timeout is reached during send_receive,
	 *   client is considered disconnected. Default is 500 msec.
	 */
};


/* ================================ DOCA SCHEMA ================================ */
/**
 * @brief Initialize DOCA schema to prepare it for setting attributes and adding types.
 * DOCA schema is used to initialize DOCA sources that will collect the data according
 * to the same schema.
 *
 * @param schema_name
 * Name of the schema.
 * @return
 * Pointer to DOCA schema, NULL on error.
 */
__DOCA_EXPERIMENTAL
void *doca_telemetry_schema_init(const char *schema_name);


/**
 * @brief Add user-defined fields to create new type in DOCA schema.
 *
 * @param doca_schema
 * Schema to create type in.
 * @param new_type_name
 * Name for new type.
 * @param fields
 * User-defined fields.
 * @param num_fields
 *  Number of user defined fields.
 * @param type_index
 * Type index for the created type is written to this variable.
 * @return
 * 0 on success, a negative telemetry_status on error
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_schema_add_type(void *doca_schema,
				   const char *new_type_name,
				   doca_telemetry_field_info_t *fields,
				   int num_fields,
				   doca_telemetry_type_index_t *type_index);


/**
 * @brief Destructor for DOCA schema.
 *
 * @param doca_schema
 * Schema to destroy.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_schema_destroy(void *doca_schema);


/**
 * @brief Finalizes schema setup to start creating Doca Sources from the schema.
 *
 * Do NOT add new types after this function was called.
 *
 * @param doca_schema
 * Input schema to start.
 * @return
 * 0 on success, a negative telemetry_status on error
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_schema_start(void *doca_schema);


/**
 * @brief Set buffer attributes to DOCA schema.
 *
 * @param doca_schema
 * Input schema.
 * @param buffer_attr
 * Attribute to set.
 * @return
 * 0 on success, a negative telemetry_status on error
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_schema_buffer_attr_set(void *doca_schema,
					  struct doca_telemetry_buffer_attr_t *buffer_attr);


/**
 * @brief Set file write attributes to DOCA schema.
 *
 * @param doca_schema
 * Input schema.
 * @param file_attr
 * Attribute to set.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_schema_file_write_attr_set(void *doca_schema,
					       struct doca_telemetry_file_write_attr_t *file_attr);


/**
 * @brief Set ipc timeout attributes to DOCA schema.
 *
 * @param doca_schema
 * Input schema.
 * @param ipc_timeout_attr
 * Attribute to set.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_schema_ipc_timeouts_attr_set(void *doca_schema,
				struct doca_telemetry_ipc_timeout_attr_t *ipc_timeout_attr);



/**
 * @brief Set IPC tarnsport attributes to DOCA schema.
 *
 * @param doca_schema
 * Input schema.
 * @param ipc_attr
 * Attribute to set.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_schema_ipc_attr_set(void *doca_schema,
					struct doca_telemetry_ipc_attr_t *ipc_attr);


/**
 * @brief Set Opaque events attributes to DOCA shcema.
 *
 * @param doca_schema
 * Input schema.
 * @param opaque_events_attr
 * Attribute to set.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_schema_opaque_events_attr_set(void *doca_schema,
		struct doca_telemetry_opaque_events_attr_t *opaque_events_attr);

/* ================================ DOCA SOURCE ================================ */

/**
 * @brief Creates a single DOCA source from schema.
 *
 * To create a DOCA source, first call doca_telemetry_schema_start() to prepare
 * the DOCA schema.
 *
 * @param doca_schema
 * Schema from which source will be created.
 * @return
 * pointer to DOCA source, or NULL on error.
 */
__DOCA_EXPERIMENTAL
void *doca_telemetry_source_create(void *doca_schema);


/**
 * @brief Applies source attribute and starts DOCA source.
 *
 * Call this function to start reporting.
 *
 * @param doca_source
 * DOCA source to start.
 * @return
 * 0 on success, a negative telemetry_status on error
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_source_start(void *doca_source);


/**
 * @brief Report events data of the same type via DOCA source.
 *
 * Data is flushed from internal buffer when the buffer is full.
 * Flushing the data immediately can be done by invoking @ref doca_telemetry_source_flush().
 *
 * @param doca_source
 * Source to report.
 * @param index
 * Type index in the DOCA schema.
 * @param data
 * Data buffer.
 * @param count
 * Number of events written to the data buffer.
 * @return
 * 0 on success, a negative telemetry_status on error
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_source_report(void *doca_source, doca_telemetry_type_index_t index,
				 void *data, int count);

/**
 * @brief Report opaque event data via DOCA source.
 *
 * Data is flushed from internal buffer when the buffer is full.
 * Flushing the data immediately can be done by invoking @ref doca_telemetry_source_flush().
 *
 * @param doca_source
 * Source to report.
 * @param app_id
 * User defined application ID.
 * @param user_defined1
 * User defined parameter 1.
 * @param user_defined2
 * User defined parameter 2.
 * @param data
 * Data buffer.
 * @param data_size
 * Size of the data in the data buffer.
 * @return
 * 0 on success, a negative telemetry_status on error.
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_source_opaque_report(void *doca_source, const doca_guid_t app_id,
			uint64_t user_defined1, uint64_t user_defined2,
			const void *data, uint32_t data_size);

/**
 * @brief Get max data size for opaque report
 *
 * @param doca_source
 * Source to report.
 * @return
 * Maximal data size
 */
__DOCA_EXPERIMENTAL
uint32_t doca_telemetry_source_opaque_report_max_data_size(void *doca_source);


/**
 * @brief Set source attributes to DOCA.
 *
 * source_tag is set on schema basis while source_id is set on source basis.
 *
 * @param doca_source
 * Source to update.
 * @param source_attr
 * Source attribute to set.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_source_name_attr_set(void *doca_source,
					 struct doca_telemetry_source_name_attr_t *source_attr);


/**
 * @brief Destructor for DOCA source.
 *
 * @param doca_source
 * Source to destroy.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_source_destroy(void *doca_source);


/* ================================ Utilities ================================ */

/**
 * @brief Get timestamp in the proper format.
 *
 * @return
 * Timestamp value.
 */
__DOCA_EXPERIMENTAL
doca_telemetry_timestamp_t doca_telemetry_timestamp_get(void);


/**
 * @brief Immediately flush the data of the DOCA source.
 *
 * @param doca_source
 * DOCA source to flush.
 */
__DOCA_EXPERIMENTAL
void doca_telemetry_source_flush(void *doca_source);

/**
 * @brief Return status of IPC transport
 *
 * @param doca_source
 * Input doca source.
 *
 * @return
 * 1 if IPC is disabled from config.
 * @return
 * 0 (DOCA_TELEMETRY_OK) if IPC is connected.
 * @return
 * negative telemetry_status if IPC is not connected. This status occurs after data send_receive
 */
__DOCA_EXPERIMENTAL
int doca_telemetry_check_ipc_status(void *doca_source);

#ifdef __cplusplus
}
#endif

/** @} */

#endif  /* DOCA_TELEMETRY_H_ */
