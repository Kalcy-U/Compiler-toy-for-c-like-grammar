int  program(int a, int b, int c)
{
	int i;
	int j;
	i = 0;
	if (a > (b + c))
	{
		j = a + (b * c + 1);
	}
	else
	{
		j = a;
	}
	while (i <= 100)
	{
		i = j * 2;
	}
	return i;
}
