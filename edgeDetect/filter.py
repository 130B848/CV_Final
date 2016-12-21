import cv2
import numpy as np

"""
Actually, I think this assignment is also transfer math formula into code.
Therefore, the variable name is really easy to understand and there is no 
 need to cite again. x, y represents cordination, rows, cols is row and colomn.
I do not use OOP which may cause a bit difficulty to read? Sorry for my poor py.
"""

def gaussian_kernel(sigma, truncate = 4.0):
    """
    Return Gaussian that truncates at the given number of standard deviations. 
    """
    sigma = float(sigma)
    radius = int(truncate * sigma + 0.5)

    x, y = np.mgrid[-radius:radius + 1, -radius:radius + 1]
    sigma = sigma**2

    k = 2 * np.exp(-0.5 * (x**2 + y**2) / sigma)
    k = k / np.sum(k)

    return k

def mean_kernel():
    """
    Mean is very simple, just a 3x3 kernel which sum up as 1, each 1 / 9.
    [[1/9, 1/9, 1/9],
     [1/9, 1/9, 1/9],
     [1/9, 1/9, 1/9]]
    """
    tmp = 1.0 / 9
    return np.tile(tmp, (3, 3))

def median_filter(input):
    """
    Unfortunately, I cannot find any way to add median filter to convolve.
    Thus, I adopt an brute force way.
    """
    output = np.copy(input)
    
    rows = input.shape[0]
    cols = input.shape[1]
    
    for x in range(rows - 2):
        for y in range(cols - 2):
            tmp = output[x:x + 3, y:y + 3]
            output[x, y] = np.median(tmp)

    return output

def tile_and_reflect(input):
    """
    Make 3x3 tiled array. Central area is 'input', surrounding areas are
    reflected.
    """
    tiled_input = np.tile(input, (3, 3))

    rows = input.shape[0]
    cols = input.shape[1]

    # Now we have a 3x3 tiles - do the reflections. 
    # All those on the sides need to be flipped left-to-right. 
    for i in range(3):
        # Left hand side tiles
        tiled_input[i * rows:(i + 1) * rows, 0:cols] = \
            np.fliplr(tiled_input[i*rows:(i + 1)*rows, 0:cols])
        # Right hand side tiles
        tiled_input[i * rows:(i + 1) * rows, -cols:] = \
            np.fliplr(tiled_input[i*rows:(i + 1)*rows, -cols:])

    # All those on the top and bottom need to be flipped up-to-down
    for i in range(3):
        # Top row
        tiled_input[0:rows, i * cols:(i + 1) * cols] = \
            np.flipud(tiled_input[0:rows, i * cols:(i + 1) * cols])
        # Bottom row
        tiled_input[-rows:, i * cols:(i + 1) * cols] = \
            np.flipud(tiled_input[-rows:, i * cols:(i + 1) * cols])

    return tiled_input


def convolve(input, weights):
    """
    2 dimensional convolution.
    Borders are handled with reflection.
    """
    output = np.copy(input)
    tiled_input = tile_and_reflect(input)

    rows = input.shape[0]
    cols = input.shape[1]
    # Stands for half weights row. 
    hw_row = weights.shape[0] / 2
    hw_col = weights.shape[1] / 2

    # Now do convolution on central array.
    # Iterate over tiled_input. 
    for i, io in zip(range(rows, rows * 2), range(rows)):
        for j, jo in zip(range(cols, cols * 2), range(cols)):
            # The current central pixel is at (i, j)
            average = 0.0
            # Find the part of the tiled_input array that overlaps with the
            # weights array.
            overlapping = tiled_input[i - hw_row:i + hw_row + 1,
                                      j - hw_col:j + hw_col + 1]
            tmp_weights = weights
            merged = tmp_weights[:] * overlapping
            average = np.sum(merged)

            # Set new output value.
            output[io, jo] = average

    return output

img = cv2.imread("dilation.png", cv2.IMREAD_GRAYSCALE)

gaussian = gaussian_kernel(1.0, 4.0)
output_gaussian = convolve(img, gaussian)
cv2.imwrite("gaussian.png", output_gaussian)

mean = mean_kernel()
output_mean = convolve(img, mean)
cv2.imwrite("mean.png", output_mean)

median = median_filter(img)
cv2.imwrite("median.png", median)

