//This code is based on EECE.5610 - Heterogenous Computing from UMass Lowell.
// width(vec A) = height(vec B) for valid multiplaction

__kernel void multiply (
                         __global float *result,
                         int widthA,
                         int widthB,
                         int heightA,
                         int heightB,
                         __global float *inputA,
                         __global float *inputB)
{
    //get global position in the X direction
    int col = get_global_id(0);
    //get global position in the Y direction
    int row = get_global_id(1);

    float sum = 0.0f;

    for (int i = 0; i < widthA; i++){
       sum += inputA[row * widthA + i] * inputB[i * widthB + col];
     }
     result[row * widthB + col] = sum;
}
