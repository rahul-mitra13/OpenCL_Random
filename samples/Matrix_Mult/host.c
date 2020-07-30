//This code is based on EECE.5610 - Heterogenous Computing from UMass Lowell.
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <OpenCL/opencl.h>

#define MAX_SOURCE_SIZE 0x1000000
#define DEVICE_NAME_LEN 128

static char dev_name[DEVICE_NAME_LEN];

static float A[8] = {
  1.0f, 1.0f, 1.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f};

static float B[24] = {
  2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f,
  2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f,
  2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f,
  2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};

int main(){
  cl_uint platformCount;
  cl_platform_id* platforms;
  cl_device_id device_id;
  cl_uint ret_num_devices;
  cl_int ret;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;
  cl_program program = NULL;
  cl_kernel kernel = NULL;

  FILE *fp;
  char *filename = "./mult.cl";
  char *source_str;
  size_t source_size;

  int wA = 4;
  int hA = 2;
  int wB = 6;
  int hB = 4;
  int wC = wB;
  int hC = hA;

  clGetPlatformIDs(1, NULL, &platformCount);
  platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id*) * platformCount);
  clGetPlatformIDs(platformCount, platforms, NULL);
  ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
  ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, DEVICE_NAME_LEN, dev_name, NULL);
  printf("Device name = %s\n", dev_name);
}
