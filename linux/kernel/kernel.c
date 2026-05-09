#include <stdio.h>

static void stub_init(void)
{
	puts("[kernel-stub] init");
}

int main(void)
{
	puts("simpleos-linux-kernel-stub");
	stub_init();
	return 0;
}
