# SPDX-License-Identifier: GPL-2.0
HOST_EXTRACFLAGS += -I$(srctree)/tools/include
hostprogs-y := ainuxd ai-diagnose
ainuxd-objs := ainuxd.o ollama_client.o

include ../../scripts/Makefile.host
