# Visual Studio version and operating system
Visual Studio 2022, Windows OS

# Working Principle

blur is a Gaussian filter that blurs an image.
combine is to combine the blurred image with the original image

## Procedure
1. Create fbo, render the scene to colorattachment0, and store the colors with luminance greater than 1 into colorattachment1.
   
2. Use Gaussian filter to filter the colorattachment, note that this shader only performs unidirectional (horizontal/vertical) filtering, according to the Gaussian filter's
According to the separable feature of Gaussian filtering, you need to alternate the filtering process. Repeat this process 20 times to get an ideal bloom effect.

3. Use combineshader to superimpose the filtered effect and the original effect together, i.e. to realize the final bloom.

## Youtube video Link
https://youtu.be/qacoU8qks2Y


