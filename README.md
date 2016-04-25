# Halide Mocap

Playing around with computing optical flow using Halide.

## Compilation notes

Right now you have to do cross-compilation by running

```bash
make include/block_match_flow_arm32.o
```

on a non-ARM machine, because the Halide compiler doesn't run on ARM.

Then, you run

```bash
make bin/block_match_flow_arm32
```

on an ARM machine (e.g. TK1).

Right now the seconds step only works on the TK1, but I believe it could all be done on the non-ARM machine. 
