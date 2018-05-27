# ##############################################################################
# User variables

# user variables can be specified in the environment or in a .config file
-include .config

# Where is the LITMUS^RT userspace library source tree?
LIBLITMUS ?= ../liblitmus

# Include default configuration from liblitmus
# Liblitmus must have been built before ft_tools can be built.
include ${LIBLITMUS}/inc/config.makefile

# all sources
vpath %.c src/

# local include files
CPPFLAGS += -Iinclude/

# ##############################################################################
# Targets

all = ftcat ft2csv ftdump ftsort st-dump st-dag-response st-e2e-response st-job-stats

.PHONY: all clean
all: ${all}
clean:
	rm -f ${all} *.o *.d

obj-ftcat = ftcat.o timestamp.o
ftcat: ${obj-ftcat}

obj-ft2csv  = ft2csv.o timestamp.o mapping.o
ft2csv: ${obj-ft2csv}

obj-ftdump  = ftdump.o timestamp.o mapping.o
ftdump: ${obj-ftdump}

obj-ftsort = ftsort.o timestamp.o mapping.o
ftsort: ${obj-ftsort}

obj-st-dump = stdump.o load.o eheap.o util.o
st-dump: ${obj-st-dump}
	$(CC) $(LDFLAGS) -o $@ ${obj-st-dump}  # $(LOADLIBES) $(LDLIBS)

obj-st-dag-response = stdag_response.o load.o eheap.o util.o
st-dag-response: ${obj-st-dag-response}
	$(CC) $(LDFLAGS) -o $@ ${obj-st-dag-response}  # $(LOADLIBES) $(LDLIBS)

obj-st-e2e-response = ste2e_response.o load.o eheap.o util.o
st-e2e-response: ${obj-st-e2e-response}
	$(CC) $(LDFLAGS) -o $@ ${obj-st-e2e-response}  # $(LOADLIBES) $(LDLIBS)

obj-st-job-stats = job_stats.o load.o eheap.o util.o
st-job-stats: ${obj-st-job-stats}
	$(CC) $(LDFLAGS) -o $@ ${obj-st-job-stats} #  $(LOADLIBES) $(LDLIBS)

# dependency discovery
include ${LIBLITMUS}/inc/depend.makefile
