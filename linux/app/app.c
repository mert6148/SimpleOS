#include <stdio.h>

void app_main(void)
{
	puts("[app] app_main()");
}

int main(void)
{
	puts("simpleos-linux-app");
	app_main();
	return 0;
}
