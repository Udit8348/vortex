# BF16 Extension Results

bf16 offers less precision than f32. ULP is used to verify the accuracy of float operations.
```console
*** almost_equal_ulp: a=1.9922, b=2, ulp=ffc0, ia=3fff0040, ib=40000000
```

Here we can see the ULP for a bf16 operation is 0xffc0 = **65472**. The default ULP that passes with f32 is **6**.
This *looks* like a pretty bad difference, but it's unclear *how* bad it is. Currently, the ULP threshold is set to 70,000.

We could also use a floating point epsilon to set the accuracy threshold for floats. There is already a funciton for this.

