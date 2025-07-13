
#ifndef OCP_CONFIG_H_
#define OCP_CONFIG_H_

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
/* Trace configurations */
#define OCP_TRACE_CONFIG_TRACE_NAME_MAX_LEN		20

typedef enum{
	OCP_TRACE_1 = 0,
	OCP_TRACE_END
}ocpTraceIDs_t;

/* Controller configurations */
#define OCP_CS_CONFIG_CS_NAME_MAX_LEN		20

typedef enum{
	OCP_CS_1 = 0,
	OCP_CS_END
}ocpCSIDs_t;
//=============================================================================

#endif /* OCP_CONFIG_H_ */
