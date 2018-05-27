#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "load.h"
#include "sched_trace.h"
#include "eheap.h"

static void usage(const char *str)
{
	fprintf(stderr,
		"\n  USAGE\n"
		"\n"
		"    stdump [opts] <file.st>+\n"
		"\n"
		"  OPTIONS\n"
		"     -r         -- find task system release and exit\n"
		"     -f         -- use first non-zero event as system release\n"
		"                   if no system release event is found\n"
		"     -c         -- display a count of the number of events\n"
		"\n\n"
		);
	if (str) {
		fprintf(stderr, "Aborted: %s\n", str);
		exit(1);
	} else {
		exit(0);
	}
}

#define MAX_CONCURRENT_JOBS 30
#define OPTSTR "rcfhs:e:"

int main(int argc, char** argv)
{
	unsigned int count;
	struct heap *h;
	struct heap_node *hn, *first = NULL;
	u64 time;
    //u64 first_time = 0;
    u64 start_time[MAX_CONCURRENT_JOBS];
    size_t start_idx = 0;
    size_t end_idx = 0;
    int start_pid = 0;
    int end_pid = 0;
	struct st_event_record *rec;
	int find_release = 0;
	int show_count = 0;
	int use_first_nonzero = 0;
	int opt;

	while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
		switch (opt) {
		case 'r':
			find_release = 1;
			break;
		case 'c':
			show_count = 1;
			break;
		case 'f':
			use_first_nonzero = 1;
			break;
		case 'h':
			usage(NULL);
			break;
        case 's':
            start_pid = atoi(optarg);
            break;
        case 'e':
            end_pid = atoi(optarg);
            break;
		case ':':
			usage("Argument missing.");
			break;
		case '?':
		default:
			usage("Bad argument.");
			break;
		}
	}


	h = load(argv + optind, argc - optind, &count);
	if (!h)
		return 1;
	if (show_count)
		printf("Loaded %u events.\n", count);

	while ((hn = heap_take(earlier_event, h))) {
		time =  event_time(heap_node_value(hn));
		if (time != 0 && !first)
			first = hn;
		time /= 1000; /* convert to microseconds */
        rec = heap_node_value(hn);
        /*
        if (rec->hdr.type == ST_RELEASE && rec->hdr.pid == start_pid && rec->hdr.job == 3) {
            first_time = time;
        } else if (rec->hdr.type == ST_COMPLETION && rec->hdr.pid == end_pid) {
            printf("%llu\n",
                    (unsigned long long) (time - 40000 * (rec->hdr.job - 3) - first_time));
        }
        */
        if (rec->hdr.job > 2  && rec->hdr.type == ST_RELEASE && rec->hdr.pid == start_pid) {
            start_time[(start_idx++) % MAX_CONCURRENT_JOBS] = time;
        } else if (start_idx > end_idx && rec->hdr.type == ST_COMPLETION && rec->hdr.pid == end_pid) {
            printf("%llu\n",
                    (unsigned long long) (time - start_time[(end_idx++) % MAX_CONCURRENT_JOBS]));
        }
        /*
		if (!find_release) {
			printf("[%12llu] ", (unsigned long long) time);
			print_event(heap_node_value(hn));
		} else {
			if (rec->hdr.type == ST_SYS_RELEASE) {
				printf("%6.2fms\n", rec->data.raw[1] / 1000000.0);
				find_release = 0;
				break;
			}
		}
        */
	}
	if (find_release && use_first_nonzero && first) {
		rec = heap_node_value(first);
		printf("%6.2fms\n", event_time(rec) / 1000000.0);
	}

	return 0;
}
