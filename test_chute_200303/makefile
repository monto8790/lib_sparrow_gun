HEADER		= inc/
SOURCE		= src/
MQTT		= MQTT/

all		:	chute

chute	:	main.c							\
			$(SOURCE)serial.c				\
			$(SOURCE)protocol.c				\
			$(SOURCE)mqtt.c					\
			$(MQTT)MQTTClient.c				\
			$(MQTT)MQTTConnectClient.c		\
			$(MQTT)MQTTConnectServer.c		\
			$(MQTT)MQTTDeserializePublish.c	\
			$(MQTT)MQTTFormat.c				\
			$(MQTT)MQTTLinux.c				\
			$(MQTT)MQTTPacket.c				\
			$(MQTT)MQTTSerializePublish.c	\
			$(MQTT)MQTTSubscribeClient.c	\
			$(MQTT)MQTTSubscribeServer.c	\
			$(MQTT)MQTTUnsubscribeClient.c	\
			$(MQTT)MQTTUnsubscribeServer.c
			
				gcc	-I $(HEADER)				\
				main.c							\
				$(SOURCE)serial.c				\
				$(SOURCE)protocol.c 			\
				$(SOURCE)mqtt.c					\
				$(MQTT)MQTTClient.c				\
				$(MQTT)MQTTConnectClient.c		\
				$(MQTT)MQTTConnectServer.c		\
				$(MQTT)MQTTDeserializePublish.c	\
				$(MQTT)MQTTFormat.c				\
				$(MQTT)MQTTLinux.c				\
				$(MQTT)MQTTPacket.c				\
				$(MQTT)MQTTSerializePublish.c	\
				$(MQTT)MQTTSubscribeClient.c	\
				$(MQTT)MQTTSubscribeServer.c	\
				$(MQTT)MQTTUnsubscribeClient.c	\
				$(MQTT)MQTTUnsubscribeServer.c	\
				-o chute	-lm		-lpthread	 -Os
				
clean 	:	
				rm -rf *o chute