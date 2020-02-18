# Cray Tracer
A simple path tracer


## Path Tracing
Here's a cool animation of two metals objects, a diffuse object, a glass (dielectric) object and an emissive light source!

![Path Tracing](example_pictures/path_rotate.gif)


## Sampling
Monte Carlo integration with one sample will have a lot of noise. Here's a comparison of 16x AA, 1 sample vs. 16x AA, 256 samples:

![1 Sample](example_pictures/256samples.bmp)
![256 Samples](example_pictures/4ksamples.bmp)

## HDRI
To add backgrounds to the scenes, an HDRI was spherically mapped to an infinite-radius sphere. Radience information can then be extracted from the HDRI (with your favourite tone-mapping function) to create a lit scenen. Here's an example:

[HDRI](example_pictures/hdri_compressed.png)
