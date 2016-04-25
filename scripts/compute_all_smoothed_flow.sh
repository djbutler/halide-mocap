#!/bin/bash
for DIR in data/*;
do
    echo $DIR
    ./bin/block_match_flow $DIR
    ./apps/cross_bilateral_grid/filter ${DIR}/flow_x.png ${DIR}/0.png ${DIR}/flow_x_smooth.png 0.05 10
    ./apps/cross_bilateral_grid/filter ${DIR}/flow_y.png ${DIR}/0.png ${DIR}/flow_y_smooth.png 0.05 10
done
