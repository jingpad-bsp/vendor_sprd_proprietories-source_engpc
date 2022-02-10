#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "sprd_fts_type.h"
#include "sprd_fts_log.h"

#define __USE_GNU
#define _GNU_SOURCE
#include <math.h>
#include "unistd.h"
#include <pthread.h>
#include <sched.h>

/* map the FORTRAN math functions, etc. to the C versions */
#define DSIN    sin
#define DCOS    cos
#define DATAN   atan
#define DLOG    log
#define DEXP    exp
#define DSQRT   sqrt
#define IF      if

#define THREAD_MAX_NUM 10

#define AT_BENCHMARK "AT+BENCHMARK="
int exec_time;
float ucpubench_result[10];

/* function prototypes */
static void benchmark(void* arg)
{
	/* Math module parameters in this version */
	long I;
	long N1, N2, N3, N4, N6, N7, N8, N9, N10, N11;
	double X1, X2, X3, X4, X, Y, Z, T, T1, T2, E1[5];
	long LOOP;
	int J, K, L, II, JJ, loopstart;
	int timeout = exec_time?exec_time:10;

	/* Time parameter */
	struct timeval start,end;
	double timeuse,alltime;
	float KIPS;
	/*CPU affinity*/
	cpu_set_t mask;
	int *a = (int *)arg;

	CPU_ZERO(&mask);
	CPU_SET(*a,&mask);

	if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
		ENG_LOG("Ucpubench warning: could not set CPU%d affinity, continuing...\n", (int) *a+1);
	}

	loopstart = 10000;	  /* see the note about LOOP below */
	alltime = 0;

LCONT:
	//Start benchmark timing at this point.
	gettimeofday( &start, NULL );

	//The actual benchmark starts here.
	T  = .499975;
	T1 = 0.50025;
	T2 = 2.0;
	LOOP = loopstart;
	II	 = 1;
	JJ = 1;

IILOOP:
	N1 = 0;
	N2 = 12 * LOOP;
	N3 = 14 * LOOP;
	N4 = 345 * LOOP;
	N6 = 210 * LOOP;
	N7 = 32 * LOOP;
	N8 = 899 * LOOP;
	N9 = 616 * LOOP;
	N10 = 0;
	N11 = 93 * LOOP;
	//Module 1: Simple identifiers
	X1 =  1.0;
	X2 = -1.0;
	X3 = -1.0;
	X4 = -1.0;
	for (I = 1; I <= N1; I++) {
		X1 = (X1 + X2 + X3 - X4) * T;
		X2 = (X1 + X2 - X3 + X4) * T;
		X3 = (X1 - X2 + X3 + X4) * T;
		X4 = (-X1+ X2 + X3 + X4) * T;
	}

	//Module 2: Array elements
	E1[1] =  1.0;
	E1[2] = -1.0;
	E1[3] = -1.0;
	E1[4] = -1.0;
	for (I = 1; I <= N2; I++) {
		E1[1] = ( E1[1] + E1[2] + E1[3] - E1[4]) * T;
		E1[2] = ( E1[1] + E1[2] - E1[3] + E1[4]) * T;
		E1[3] = ( E1[1] - E1[2] + E1[3] + E1[4]) * T;
		E1[4] = (-E1[1] + E1[2] + E1[3] + E1[4]) * T;
	}

	//Module 3: Array as parameter
	for (I = 1; I <= N3; I++){
		for(J = 0; J < 6; J++){
			E1[1] = ( E1[1] + E1[2] + E1[3] - E1[4]) * T;
			E1[2] = ( E1[1] + E1[2] - E1[3] + E1[4]) * T;
			E1[3] = ( E1[1] - E1[2] + E1[3] + E1[4]) * T;
			E1[4] = (-E1[1] + E1[2] + E1[3] + E1[4]) / T2;
		}
	}

	//Module 4: Conditional jumps
	J = 1;
	for (I = 1; I <= N4; I++) {
		if (J == 1)
			J = 2;
		else
			J = 3;

		if (J > 2)
			J = 0;
		else
			J = 1;

		if (J < 1)
			J = 1;
		else
			J = 0;
	}

	//Module 5: Omitted
	//Module 6: Integer arithmetic
	J = 1;
	K = 2;
	L = 3;
	for (I = 1; I <= N6; I++) {
		J = J * (K-J) * (L-K);
		K = L * K - (L-J) * K;
		L = (L-K) * (K+J);
		E1[L-1] = J + K + L;
		E1[K-1] = J * K * L;
	}

	//Module 7: Trigonometric functions
	X = 0.5;
	Y = 0.5;
	for (I = 1; I <= N7; I++) {
		X = T * DATAN(T2*DSIN(X)*DCOS(X)/(DCOS(X+Y)+DCOS(X-Y)-1.0));
		Y = T * DATAN(T2*DSIN(Y)*DCOS(Y)/(DCOS(X+Y)+DCOS(X-Y)-1.0));
	}

	//Module 8: Procedure calls
	X = 1.0;
	Y = 1.0;
	Z = 1.0;
	for (I = 1; I <= N8; I++){
		X = T * (X + Y);
		Y = T * (X + Y);
		Z  = (X + Y) / T2;
	}

	//Module 9: Array references
	J = 1;
	K = 2;
	L = 3;
	E1[1] = 1.0;
	E1[2] = 2.0;
	E1[3] = 3.0;
	for (I = 1; I <= N9; I++){
		E1[J] = E1[K];
		E1[K] = E1[L];
		E1[L] = E1[J];
	}

	//Module 10: Integer arithmetic
	J = 2;
	K = 3;
	for (I = 1; I <= N10; I++) {
		J = J + K;
		K = J + K;
		J = K - J;
		K = K - J - J;
	}

	//Module 11: Standard functions
	X = 0.75;
	for (I = 1; I <= N11; I++)
		X = DSQRT(DEXP(DLOG(X)/T1));

	 //THIS IS THE END OF THE MAJOR LOOP.
	if (++JJ <= II)
		goto IILOOP;

	//Stop benchmark timing at this point.
	gettimeofday(&end, NULL);
	timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	timeuse /= 1000000;
	alltime += timeuse;

/*
C----------------------------------------------------------------
C	  Performance in ucpubench KIP's per second is given by
C
C	(100*LOOP*II)/TIME
C
C	  where TIME is in seconds.
C--------------------------------------------------------------------
*/

	if (timeuse <= 0) {
		ENG_LOG("Ucpubench Insufficient duration- Increase the LOOP count\n");
		return;
	}

	if (alltime <= timeout)
		goto LCONT;

	ENG_LOG("CPU%d Ucpubench Loops: %ld, Iterations: %d, Duration: %lf sec.\n",
			 (int)*a+1, LOOP, II, alltime);

	KIPS = (100.0*LOOP*II)/(float)(timeuse);
	ENG_LOG("CPU%d BENCH-ucpubench: %.1f MIPS\n", (int)*a+1, KIPS/1000.0);
	ucpubench_result[(int)*a] = KIPS/1000.0;
}

#define DIV_UCPUBENCH(a,b) fabs(a-b) / (a>b?a:b)
static int ucpubench_result_check(int cpu_num, int diff)
{
	int i, j;
	float tmp;
	for (i = 0; i < cpu_num-1; i++) {
		for (j = i + 1; j < cpu_num; j++)
			if(ucpubench_result[i] && ucpubench_result[j]) {
				tmp = DIV_UCPUBENCH(ucpubench_result[i], ucpubench_result[j]);
				if (tmp*100 > diff) {
					return ucpubench_result[i] > ucpubench_result[j]?(j+1):(i+1);
				}
			}
	}
	return 0;
}

static int benchmark_test (char *req, char *rsp)
{
	int ret, ret_thrd;
	void *retval;
	int cpu_number = sysconf(_SC_NPROCESSORS_CONF);
	pthread_t *thread;
	int tid[THREAD_MAX_NUM];
	char buff[256];
	int buff_offset;
	char *ptr = NULL;
	int nlen = 0;
	bool is_at_cmd = true;
	bool pthread_flag = true;
	int diff_value, abnormal_cpu;
	int i,j;

	if (NULL == req) {
		ALOGE("%s,null pointer", __FUNCTION__);
		if (rsp != NULL) {
			sprintf(rsp, "\r\nERROR\r\n");
			return strlen(rsp);
		} else {
			return 0;
		}
	}

	if (req[0] == 0x7e) {
		ptr = req + 1 + sizeof(MSG_HEAD_T);
		is_at_cmd = false;
	} else {
		ptr = strdup(req);
	}

	exec_time = 0;
	diff_value = 0;
	abnormal_cpu = 0;
	memset(&ucpubench_result, 0 ,sizeof(int));
	if (strncasecmp(ptr, AT_BENCHMARK, strlen(AT_BENCHMARK)) == 0) {
		nlen = strlen(ptr) - strlen(AT_BENCHMARK);
		char *ptrpara = ptr + strlen(AT_BENCHMARK);
		for (i = 0; i < nlen; i++) {
			if (ptrpara[i] == 't' || ptrpara[i] == 'T') {
				for (j=1; j < nlen; j++) {
					if (ptrpara[i+j] >= '0' && ptrpara[i+j] <= '9') {
						exec_time = exec_time*10 + (ptrpara[i+j] - '0');
					}
					else
						break;
				}
			}
			if (ptrpara[i] == 'd' || ptrpara[i] == 'D') {
				for (j=1; j < nlen; j++) {
					if (ptrpara[i+j] >= '0' && ptrpara[i+j] <= '9') {
						diff_value = diff_value*10 + (ptrpara[i+j] - '0');
					}
					else
						break;
				}
			}
		}
	}

	diff_value = diff_value?diff_value:5;
	if (is_at_cmd && ptr != NULL) {
		free(ptr);
	}

	ENG_LOG("ucpubench benchmark test :enter function test :AT+BENCHMARK!");
	ENG_LOG("Ucpubench Create %d thread for benchmark test!\n", cpu_number);
	memset(tid, -1, sizeof(tid));
	thread = (pthread_t *)malloc(sizeof(pthread_t)*cpu_number);
	for (i = 0; i < cpu_number; i++)
	{
		tid[i] = i;
		ret_thrd = pthread_create(&thread[i], NULL, (void *)&benchmark, (void *) &tid[i]);
		if (ret_thrd == 1) {
			pthread_flag = false;
			ENG_LOG("Ucpubench thread%d create failed!\n", i);
			exit(1);
		}
	}

	for (i = 0; i < cpu_number; i++) {
		ret = pthread_join(thread[i], &retval);
		if (ret != 0) {
			pthread_flag = false;
			ENG_LOG("ucpubench%d failed return value(ret) is %d\n", i, ret);
		}
	}

	if (!pthread_flag) {
		sprintf(rsp, "%s", "BENCHMARK create or run failed!Please check logcat in details.\n");
		ENG_LOG("Ucpubench Benchmark test failed!\n");
	} else {
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "%s", "Ucpubench score:\n");
		strcat(rsp, buff);
		memset(buff, 0, sizeof(buff));
		buff_offset = 0;

		for (i = 0; i < cpu_number; i++) {
			sprintf(buff + buff_offset, "CPU%d: {%.1f};\n", i+1, ucpubench_result[i]);
			buff_offset = strlen(buff);
		}

		strcat(rsp, buff);
		strcat(rsp, "\r\n");
		abnormal_cpu = ucpubench_result_check(cpu_number, diff_value);
		memset(buff, 0, sizeof(buff));
		if (abnormal_cpu == 0) {
			sprintf(buff, "Ucpubench Benchmark test Pass. \r\n");
			ENG_LOG("Ucpubench Benchmark test %d seconds end!\n", exec_time?exec_time:10);
		} else {
			sprintf(buff, "Ucpubench Benchmark CPU%d failed. \n", abnormal_cpu);
			ENG_LOG("Ucpubench Benchmark test Failed.\r\n");
		}
		strcat(rsp, buff);
	}
	return strlen(rsp);;
}
void register_this_module_ext(struct eng_callback *reg, int *num)
{
	int moudles_num = 0;
	ENG_LOG("register_this_module_ext :ucpubench benchmark test");

	//1st command
	sprintf(reg->at_cmd, "%s", AT_BENCHMARK);
	reg->eng_linuxcmd_func = benchmark_test;
	moudles_num++;

	*num = moudles_num;
	ENG_LOG("Ucpubench register_this_module_ext: %d - %d",*num, moudles_num);
}
