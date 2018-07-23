#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := atca

PATH += :/home/milan/esp32/xtensa-esp32-elf/bin
IDF_PATH = /home/milan/esp32/esp-idf


include $(IDF_PATH)/make/project.mk

