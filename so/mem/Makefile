TOPDIR = $(shell pwd)
OUTDIR = $(TOPDIR)/bin

CC = gcc
CXX = g++
#CFLAGS = -std=c++11 -MMD -MP -Wall -W -Werror -Wno-sign-compare -Wno-unused-result
CFLAGS = -std=c++11 -MMD -MP -Wall -Werror -Wno-sign-compare -Wno-unused-result
LDFLAGS =

ifeq ($(CXX), clang++)
CFLAGS += -Wno-unused-private-field
endif

ifeq ($(DEBUG), 1)
CFLAGS += -g --coverage -DDEBUG
LDFLAGS += --coverage
else
CFLAGS += -O2
endif

SUBDIR = source

export CC CXX CFLAGS LDFLAGS OUTDIR TOPDIR

ifeq ($(mod),)
all:
	@[ ! -e $(OUTDIR) ] &  mkdir -p $(OUTDIR)
	for dir in $(SUBDIR); do \
		make -C $$dir all || exit 1;   \
	done

clean:
	rm $(OUTDIR)/* -r
	for dir in $(SUBDIR); do  \
		make -C $$dir clean;  \
	done

else
all:
	@[ ! -e $(OUTDIR) ] &  mkdir -p $(OUTDIR)
	make -C $(mod) all

clean:
	make -C $(mod) clean

endif

.PHONY: all clean
