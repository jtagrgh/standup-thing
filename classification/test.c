#include "analyze.h"
#include "stdio.h"
#include "average.h"
#include "integral.h"
#include "integral_3d.h"
#include "adjust_acceleration.h"


int main(int argc, char *argv[]) {
	
	{
		double values[] = {1, 2, 3, 4, 5};
		AverageState state;
		init_average_state(&state, 1);
		for (int i = 0; i < 5; i++) {
			average(&state, values[i]);
			printf("\n");
		}
		printf("Average: %f, N_samples %d \n", state.value, state.n_samples);
		printf("\n");
	}

	{
		double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		IntegralState integral_state;
		init_integral_state(&integral_state, 1, 1);
		for (int i = 0; i < 10; i++) {
			integral(&integral_state, values[i]);
			printf("\n");
		}
		printf("Integral: %f, N_samples %d \n", integral_state.value, integral_state.n_samples);
		printf("\n");
	}

	{
		Vec3d values[] = {
			{.x = 1, .y = 1, .z = -1},
			{.x = 2, .y = 2, .z = -2},
			{.x = 3, .y = 3, .z = -3},
			{.x = 4, .y = 4, .z = -4},
			{.x = 5, .y = 5, .z = -5}
		};
		Integral3dState integral_state;
		init_integral_3d_state(&integral_state, 1, 1);
		for (int i = 0; i < 5; i++) {
			integral_3d(&integral_state, values[i]);
			printf("\n");
		}
		printf("Integral 3D: %f, %f, %f, N_samples %d \n", integral_state.value.x, integral_state.value.y, integral_state.value.z, integral_state.x.n_samples);
		printf("\n");
	}

	/* Sample from python.
		           time        ax        ay        az        gx        gy        gz  \
		Data set                                                                      
		0         13542 -2.851494 -2.366668  9.566033  0.000000  0.000000  0.000000   
		1         13677 -2.873042 -2.331952  9.624092  0.000000  0.000000  0.000000   
		2         13813 -2.883816 -2.209848  9.517550  0.000000  0.000000  0.000000   
		3         13948 -2.948459 -2.255338  9.443330  0.000000  0.000000  0.000000   
		4         14083 -2.756325 -2.263119  9.430760  0.000000  0.000000  0.000000   
		5         14219 -2.792238 -2.132635  9.355343  0.007507 -0.005859 -0.004700   
		6         14354 -2.805406 -2.013524  9.230845  0.007812 -0.006042 -0.004272   
		7         14490 -2.800019 -2.062605  9.185953  0.009155 -0.006409 -0.003601   
		8         14625 -2.850896 -2.227805  9.183559  0.010254 -0.004883 -0.002014   
		9         14760 -2.823961 -2.297236  9.060258  0.010803 -0.003235 -0.000061   
	*/

	{
		BMI2SensAxisData values[] = {
			{.x = -4764, .y = -3954, .z = 15982},
			{.x = -4800, .y = -3896, .z = 16079},
			{.x = -4818, .y = -3692, .z = 15901},
			{.x = -4926, .y = -3768, .z = 15777},
			{.x = -4605, .y = -3781, .z = 15756},
			{.x = -4665, .y = -3563, .z = 15630},
			{.x = -4687, .y = -3364, .z = 15422},
			{.x = -4678, .y = -3446, .z = 15347},
			{.x = -4763, .y = -3722, .z = 15343},
			{.x = -4718, .y = -3838, .z = 15137},
		};

		AdjustAccelerationState acc_adjusted;
		init_adjust_acceleration_state(&acc_adjusted, 1);
		for (int i = 0; i < 10; i++) {
			adjust_acceleration(&acc_adjusted, values[i]);
			printf("\n");
		}
		printf("Adjusted acceleration: %f, %f, %f \n", acc_adjusted.value.x, acc_adjusted.value.y, acc_adjusted.value.z);
		printf("\n");
	}

	{

	}

	return 0;
}