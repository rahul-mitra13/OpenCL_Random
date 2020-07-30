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
  char *fileName = "./mult.cl";
  char *source_str;
  size_t source_size;

  int wA = 4;
  int hA = 2;
  int wB = 6;
  int hB = 4;
  int wC = wB;
  int hC = hA;
  int i = 0;

  clGetPlatformIDs(1, NULL, &platformCount);
  platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id*) * platformCount);
  clGetPlatformIDs(platformCount, platforms, NULL);
  ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
  ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, DEVICE_NAME_LEN, dev_name, NULL);
  printf("Device name = %s\n", dev_name);

  context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
  command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

  fp = fopen(fileName, "r");
  if (!fp){
    fprintf(stderr, "Failed to open kernel.\n");
  }

  source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

  if (ret != CL_SUCCESS){
    printf("Failed to create program from source.\n");
    exit(1);
  }

  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  if (ret != CL_SUCCESS){
    printf("Failed to build program.\n");
    exit(1);
  }

  kernel = clCreateKernel(program, "multiply", &ret);
  if (ret != CL_SUCCESS){
    printf("Failed to create kernel.\n");
    exit(1);
  }

  float *C = (float *)calloc(hC * wC, sizeof(float));
  for (i = 0; i < hC * wC; i++){
    printf("%f ", C[i]);
  }
  printf("\n");

  //A
  cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, wA*hA*sizeof(float), NULL, &ret);
  clEnqueueWriteBuffer(command_queue, bufferA, CL_TRUE, 0, wA*hA*sizeof(float), (void *)A, 0, NULL, NULL);
  //B
  cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, wB*hB*sizeof(float), NULL, &ret);
  clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, wB*hB*sizeof(float), (void *)B, 0, NULL, NULL);
  //C
  cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, wC*hC*sizeof(float), NULL, &ret);
  clEnqueueWriteBuffer(command_queue, bufferC, CL_TRUE, 0, wC*hC*sizeof(float), (void *)C, 0, NULL, NULL);

  //set kernel arguments
  clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&bufferC);
  clSetKernelArg(kernel, 1, sizeof(cl_int), (void *)&wA);
  clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&wB);
  clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&hA);
  clSetKernelArg(kernel, 4, sizeof(cl_int), (void *)&hB);
  clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&bufferA);
  clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&bufferB);

  size_t globalws[2] = {wC, hC};
  size_t localws[2] = {2, 2};
  ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalws, localws, 0, NULL, NULL);
  if (ret != CL_SUCCESS){
    printf("Failed to EnqueueNDRangeKernel.\n");
    exit(1);
  }

  clEnqueueReadBuffer(command_queue, bufferC, CL_TRUE, 0, wC*hC*sizeof(float), (void *)C, 0, NULL, NULL);

  for (i = 0; i < wC * hC; i++){
    printf("%f ",C[i]);
  }
  printf("\n");

  //free resources
  free(C);

  clReleaseMemObject(bufferA);
  clReleaseMemObject(bufferB);
  clReleaseMemObject(bufferC);
  clReleaseCommandQueue(command_queue);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseContext(context);
}
