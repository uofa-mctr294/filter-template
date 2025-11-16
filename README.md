# image-filtering

## Introduction

Signal processing is a common problem domain in many fields of engineering, and it is especially pervasive in Electrical & Computer Engineering, Mechatronics included. Many phenomena can be expressed as signals, including videos, music, radio, and sensor data. Even seemingly unrelated disciplines can make use of signal processing.

A staple subdomain of signal processing is that of 2D positional signals—more colloquially known as images. The theory and details of image processing are long and complicated, but knowledge of image processing enables applications that include image visualization, compression, photo editing, computer vision, generative AI, and more. In addition, skills in image processing transfer readily to other domains.

One of the most fundamental methods of image processing is filtering. Filtering is used to manipulate images as well as extract features. For these reasons it is often the first step in an image-processing pipeline. Using computer vision as an example, filters are used for feature extraction, and these features are then passed to learning or recognition networks that evaluate properties of the image.

Convolution kernels are commonly used to perform image filtering. They have the benefit of being linear operators and are easily expressed as small 2D matrices. Filters can be used for blurring, sharpening, edge detection, and more.

## Requirements

- You are given a path to an image for processing in a Portable Gray Map (PGM) format.
- Read the contents of the image
- Apply filtering to the image
  - Apply a Sobel horizontal edge filter to create an image `Gx`
  - Apply a Sobel vertical edge filter to create an image `Gy`
  - Combine the two images to create a normalized gradient image `G`
- Write the created gradient images to a new file

## Testing Requirements

- For the sake of testing we require that all your source files are placed in `src`.
- We also require that nothing in `tests/test_filter.cpp` is removed. You may feel free to add to this file or add additional files to this directory, as long as testing continues to build and run. Any additional changes may require you to make changes to `CMakeLists.txt`
- It is expected that the following (and similar) commands will run correctly:
  - `'cmd' image\boats.pgm image\boats-gradient`
where 'cmd' is the path to your produced binary (retrieving this path will be handled by `CMakeLists.txt`), the first argument is a relative path to an input image, and the second is a relative path to an output image base filename. For the given example the program will output three files: `image\boats-gradient-hedge.pgm`, `image\boats-gradient-vedge.pgm`, and `image\boats-gradient-magedge.pgm`.
- Note: typically *relative* paths like the ones above are evaluated relative to the 'current working directory' (cwd) which is **not necessarily** the folder the exe is in; rather it is the directory it is run from.

## Bonus

- Some 2D filters are [separable](https://en.wikipedia.org/wiki/Separable_filter), meaning that they can be expressed as two separate 1D filters.
  - Perform the above filtering using both a 2D filter and two consecutive 1D filters, and measure the performance difference

### Hints

- The PGM format specification is provided [here](https://netpbm.sourceforge.net/doc/pgm.html), but its specification is simple enough to be reverse engineered by inspection.
- A list of some examples of image kernels is provided [here](https://www.geeksforgeeks.org/deep-learning/types-of-convolution-kernels/).
- An example of 2D convolution can be found [here](https://songho.ca/dsp/convolution/convolution2d_example.html).
- For a partial differential equation, the normalized magnitude of the gradient can be expressed as `G = sqrt(Gx^2 + Gy^2)`

## Building

```shell
cmake -S . -B build
cmake --build build --config Debug
build\Debug\filter.exe image/boats.pgm image/boats-gradient
```

## Testing

[Build](#building)

```shell
ctest --test-dir build -C Debug
Add -V for verbose testing
```
