# Know how to do 
- render flow color instead of separate x- and y-channels
- try using all three color channels for SSD
- splat the cost volumes of all pixels onto the bilateral grid, take argmax of each bilateral vertex, and slice back
- get a simple version running on iPhone 6s: grabbing pairs of frames at 240fps, doing block matching, and displaying result
    - start here: https://github.com/halide/Halide/tree/master/apps

# Less sure how to do
- use cross-bilateral filtering to smooth and in-paint flow based on RGB
- compute flow uncertainty (ratio of 1st- to 2nd-lowest score? distance from 1st to 2nd?)

# Unsure how to do
- use region-growing to segment foreground object
- propgate segment from frame-to-frame to *improve* beyond pairwise flow estimates
    - NOTE: according to Richard, this will probably require well-calibrated uncertainty scores and a sound update rule
- adaptively select the max window displacement (binary search, h264-style?) to speed up matching

