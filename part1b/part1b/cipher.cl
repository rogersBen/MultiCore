__kernel void cipher(__global char *in, __global char *out) {

	int num = get_global_id(0);
	out[num] = in[num] - 3;
	
}