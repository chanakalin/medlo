# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/paho.mqtt.embedded-c/MQTTClient-C/src $(IDF_PATH)/components/paho.mqtt.embedded-c/MQTTPacket/src $(IDF_PATH)/components/paho.mqtt.embedded-c/MQTTClient-C/src/linux
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/paho.mqtt.embedded-c -lpaho.mqtt.embedded-c
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += paho.mqtt.embedded-c
component-paho.mqtt.embedded-c-build: 
