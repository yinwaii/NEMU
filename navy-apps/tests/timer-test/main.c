#include <stdio.h>
#include <NDL.h>
// #include <sys/time.h>
#include <time.h>
int main()
{
	printf("Begin:\n");
	uint32_t start = NDL_GetTicks();
	int sec = 0;
	while (1)
	{
		// printf("%d\n", io_read(AM_TIMER_UPTIME).us / 1000000);
		while ((NDL_GetTicks() - start) / 1000 < sec)
			;
		printf("%ds passed %ld\n", sec, NDL_GetTicks());
		sec++;
	}
}