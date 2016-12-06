#ifndef EVENTSENDER_H
#define EVENTSENDER_H
#include <QObject>

class EventSender: public QObject
{
	Q_OBJECT
public:
	explicit EventSender(QObject *parent = 0);
	virtual ~EventSender(){}
public:
	void HideSignal( bool hide );

signals:
	void setHideEvent( bool hide );
};

#endif // EVENTSENDER_H
