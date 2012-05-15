# $Id: GNUmakefile,v 1.11 2006-12-08 09:30:24 vnivanch Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

name := music
G4TARGET := $(name)
G4EXLIB := true
G4WORKDIR := $PWD

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

.PHONY: all
all:  lib bin

CPPFLAGS += $(shell $(ROOTSYS)/bin/root-config --cflags) -g
ROOTLIBS = $(shell $(ROOTSYS)/bin/root-config --glibs)
EXTRALIBS += $(ROOTLIBS)


include $(G4INSTALL)/config/binmake.gmk

