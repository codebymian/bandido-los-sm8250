#!/bin/bash
# Build script for sm8250 with AnyKernel3 packaging

KERNEL_DIR=$(pwd)
DEVICE="$1"
TOOLCHAIN_DIR="$2"
OUT_DIR="out/$DEVICE"

export PATH="$TOOLCHAIN_DIR/bin:$PATH"
export CC="ccache clang"
export ARCH=arm64
export LLVM=1
export LLVM_IAS=1
export CROSS_COMPILE=aarch64-linux-gnu-
export CROSS_COMPILE_ARM32=arm-linux-gnueabi-
export PLATFORM_VERSION=11
export KCFLAGS="-Wno-error=pointer-to-enum-cast \
                -Wno-error=int-conversion \
                -Wno-unused-variable \
                -Wno-unused-function"

DEFCONFIGS="arch/$ARCH/configs/vendor/kona-sec-perf_defconfig \
            arch/$ARCH/configs/vendor/samsung/${DEVICE}.config \
            arch/$ARCH/configs/ksu.config"

mkdir -p "$OUT_DIR"

# Merge defconfigs
./scripts/kconfig/merge_config.sh -m -O "$OUT_DIR" $DEFCONFIGS
make O="$OUT_DIR" olddefconfig

# Build kernel image + dtbs
make -j$(nproc) O="$OUT_DIR" Image.gz-dtb
make -j$(nproc) O="$OUT_DIR" dtbs

# Concatenate DTBs into a single dtb file
cat "$OUT_DIR/arch/arm64/boot/dts/vendor/qcom/kona.dtb" \
    "$OUT_DIR/arch/arm64/boot/dts/vendor/qcom/kona-v2.dtb" \
    "$OUT_DIR/arch/arm64/boot/dts/vendor/qcom/kona-v2.1.dtb" \
    > "$OUT_DIR/dtb"

# Build dtbo.img (grab all .dtbo files for device)
DTBO_FILES=$(find "$OUT_DIR/arch/arm64/boot/dts/samsung/$DEVICE" -name "*.dtbo")
if [ -n "$DTBO_FILES" ]; then
    "$KERNEL_DIR/tools/mkdtimg" create "$OUT_DIR/dtbo.img" --page_size=4096 ${DTBO_FILES}
else
    echo "⚠️ No DTBO files found for $DEVICE"
fi

# Package with AnyKernel3
echo "-----------------------------------------------"
echo "📦 Packaging AnyKernel3 zip for $DEVICE"
echo "-----------------------------------------------"

cd AnyKernel3/ || exit 1

# Copy kernel outputs
cp "$KERNEL_DIR/$OUT_DIR/arch/arm64/boot/Image.gz-dtb" Image.gz-dtb
cp "$KERNEL_DIR/$OUT_DIR/dtb" dtb
[ -f "$KERNEL_DIR/$OUT_DIR/dtbo.img" ] && cp "$KERNEL_DIR/$OUT_DIR/dtbo.img" dtbo.img

# Update device name in anykernel.sh
sed -i "s/^device\.name1=.*/device.name1=${DEVICE}/" anykernel.sh

# Create zip with timestamp
BUILD_DATE=$(date +"%Y-%m-%d_%H-%M")
ZIP_NAME="Bandido-Kernel-${DEVICE}-${BUILD_DATE}.zip"
zip -r "../${ZIP_NAME}" *

cd "$KERNEL_DIR"
echo "✅ Build for $DEVICE completed at: $(date +"%A, %d %B %Y %H:%M")"
echo "Output zip: $ZIP_NAME"
