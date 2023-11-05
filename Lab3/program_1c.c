# program_1c.c

double v1[64], v2[64], v3[64], v4[64];

int i, j;
int m=1; /* 64  bit */
double k=0,p;
for (i = 0; i < 63; i += 2){
	
	p_even = v1[i] * ((double)( m << i)); /*logic shift */
	j = i+1;
	m = (int)p_even;
	v6_even_pre = k + v1[i];
	k = (double)(((int)v4[j]) >> j);
	v6_odd_pre = k + v1[j];
	v7_even_pre = v2[i] + v3[i];
	v7_odd_pre = v2[j] + v3[j];

	v5[i] = ((p_even * v2[i]) + v3[i]) + v4[i];

	v6[i] = v5[i] / v6_even_pre;

	v7[i] = v6[i] * v7_even_pre;
	
	p_odd = v1[j] / ((double)m * j));
	
	v5[j] = ((p_odd * v2[j]) + v3[j]) + v4[j];

	v6[j] = v5[j] / v6_odd_pre;

	v7[j] = v6[j] * v7_odd_pre;
}
