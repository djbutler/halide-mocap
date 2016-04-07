#!/bin/bash
for DIR in data/*; do echo $DIR; ./bin/block_match_flow $DIR; done
