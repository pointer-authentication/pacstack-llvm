# PACStack

[PACStack](https://pacstack.github.io) implementation built on the [LLVM
Compiler Infrastructure](https://llvm.org) ([main LLVM
repository](https://github.com/llvm/llvm-project)).

## Usage

PACStack uses the ARMv8.3-a PAuth extension and supports only AArch64 targets
with PAuth extension (e.g., `-march=armv8.3-a`). PACStack is currently
implemented purely in the LLVM back-end and is controlled with the
`-pacstack=(none|nomask|full)` command line options. To instrument when
compiling with Clang, you can use the `-mllvm` flag to pass the option to the
back-end.

On x86 you can use the following command to cross-compile for AArch64 using
sysroot and gcc-toolchain from [Linaro](https://www.linaro.org/downloads):

```
$PATH_TO_COMPILER/clang          \
  --sysroot=$SYSROOT             \
  --gcc-toolchain=$GCC_TOOLCHAIN \
  --target=aarch64-linux-gnu     \
  -march=armv8.3a                \
  -mllvm -pacstack=full          \
  $SOURCE_FILE
```

To test compiled binaries without actual hardware you can use either the [fixed
virtual
platform](https://developer.arm.com/tools-and-software/simulation-models/fixed-virtual-platforms)
simulation models from ARM or QEMU version 4.0 (or newer).

## More information

For more on PACStack, please check
[pacstack.github.io](https://pacstack.github.io), which contains links to latest
publications and other relevenat information.
