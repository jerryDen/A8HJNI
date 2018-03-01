#include <stddef.h>
#include <malloc.h>
#include "WB_doorCard.h"
#include "hwInterface/hwInterfaceManage.h"
#include "cpuCard/cpu_card.h"
#include "icCard/fm1702nl.h"
#include "common/debugLog.h"

typedef struct {
	DoorCardops ops;
	void *cardClass;
	DOOR_CARD_MODULE type;
}DoorCardServer,*pDoorCardServer;
pDoorCardops createDoorCardServer(DOOR_CARD_MODULE type, DoorCardRecvFunc callBackFunc) {
	pDoorCardServer server = (pDoorCardServer)calloc(1,sizeof(DoorCardServer));
	if(server == NULL)
		return NULL;
	server->type = type;
	switch (type) {
	case FM1702NL:
		LOGD("start FM1702NL!\n");
		server->cardClass = crateFM1702NLOpsServer(
				crateHwInterfaceServer()->getDoorCardUART(),
				callBackFunc);
		break;
	case ZLG600A:
		LOGD("start ZLG600A!\n");
		server->cardClass = createZLG600AServer(
				crateHwInterfaceServer()->getDoorCardUART(),
				(RecvFunc) callBackFunc);
		break;
	default:
		return NULL;
		break;
	}
	return server;
}
void destroyDoorCardServer(pDoorCardops *server) {
	if(server !=NULL && *server!=NULL)
		return ;

	pDoorCardServer pthis = (pDoorCardServer)*server;

	switch (pthis->type) {
	case FM1702NL:
		destroyFM1702NLOpsServer(&pthis->cardClass);
		break;
	case ZLG600A:
		destroyZLG600AServer(&pthis->cardClass);
		break;
	default:
		break;
	}
	free(pthis);
	*server = NULL;

}
