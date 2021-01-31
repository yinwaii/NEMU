#include <stdio.h>
#include <fixedptc.h>
int main()
{
	fixedpt a = fixedpt_rconst(-1.2);
	fixedpt b = fixedpt_fromint(10);
	int c = 0;
	if (b > fixedpt_rconst(7.9))
	{
		c = fixedpt_toint(fixedpt_div(fixedpt_mul(a + FIXEDPT_ONE, b), fixedpt_rconst(2.3)));
	}
	printf("%d, %d\n", fixedpt_toint(fixedpt_mul(a + FIXEDPT_ONE, b)), c);
	a = fixedpt_rconst(-100);
	c = fixedpt_toint(fixedpt_abs(a));
	printf("%d\n", c);
	c = fixedpt_toint(fixedpt_ceil(a));
	printf("%d\n", c);
	c = fixedpt_toint(fixedpt_floor(a));
	printf("%d\n", c);
	printf("%d\n", fixedpt_toint(fixedpt_muli(fixedpt_div(fixedpt_rconst(7.1), fixedpt_rconst(30.4)), 10)));
}