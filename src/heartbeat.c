#include <logger.h>
#include <conversion.h>
#include <gatt.h>
#include <version.h>
#include <sdk_common.h>
#include "heartbeat.h"


void log_heartbeat_info(BleMessageHeartbeatReq *request)
{
  char *nodeName = malloc(NODE_NAME_SIZE);
  char *parentNodeName = malloc(NODE_NAME_SIZE);

  set_node_name_from_device_id(request->deviceId, nodeName);
  if (request->centralConnectionDeviceId == 0) {
    strncpy(parentNodeName, "ROOT", 5);
  } else {
    set_node_name_from_device_id(request->centralConnectionDeviceId, parentNodeName);
  }

  char *nfcEnabled = (request->nfc == NFC_STATUS_DISABLED) ? "false" : "true";
  char *nfcWorking = (request->nfc != NFC_STATUS_WORKING)  ? "false" : "true";

  NRF_LOG_PRINTF("HEARTBEAT: {\"id\": \"%s\", \"rawId\": %u, \"parentId\": \"%s\", \"version\": \"%u.%u\", \"votes\": %u, "
              "\"nfcEnabled\": %s, \"nfcWorking\": %s}\r\n",
      nodeName, request->deviceId, parentNodeName, request->majorVersion, request->minorVersion, request->voteCount,
      nfcEnabled, nfcWorking);

  free(nodeName);
  free(parentNodeName);
}


void broadcast_heartbeat(void *data, uint16_t dataLength)
{
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(dataLength);

  BleMessageHeartbeatReq request;
  memset(&request, 0, sizeof(request));
  request.head.messageType = Heartbeat;
  request.majorVersion = APP_VERSION_MAIN;
  request.minorVersion = APP_VERSION_SUB;
  request.deviceId = deviceId;
  if (central_connection_active() && (activeConnections->central.deviceId != 0)) {
    request.centralConnectionDeviceId = activeConnections->central.deviceId;
  }
  request.voteCount = get_vote_count();
  request.nfc = get_nfc_status();

  log_heartbeat_info(&request);
  send_to_central_connection(BLE_CONN_HANDLE_INVALID, (uint8_t *) &request, sizeof(request));
}

void receive_heartbeat(uint8_t *heartbeat) {
  log_heartbeat_info((BleMessageHeartbeatReq *)heartbeat);
}


