#include "eventsender.h"

EventSender::EventSender(QObject *parent) :
	QObject(parent)
{
}


void EventSender::HideSignal( bool hide )
{
	emit setHideEvent( hide );
}
