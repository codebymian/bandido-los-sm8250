#!/bin/bash

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

BUILD_VAR="-j$(nproc) -C $(pwd) O=$(pwd)/out ARCH=arm64 LLVM=1"

# -------------------------------
# Kernel Build
# -------------------------------
build_kernel() {
    echo "-----------------------------------------------"
    echo "Beginning kernel compilation for $DEVICE..."
    echo "-----------------------------------------------"

    # Merge defconfigs into temp_defconfig
    cat arch/arm64/configs/vendor/kona-sec-perf_defconfig \
        arch/arm64/configs/vendor/samsung/${DEVICE}.config \
        arch/arm64/configs/ksu.config > arch/arm64/configs/temp_defconfig

    # Enable ThinLTO
    echo "CONFIG_LTO=y" >> arch/arm64/configs/temp_defconfig
    echo "CONFIG_LTO_CLANG=y" >> arch/arm64/configs/temp_defconfig
    echo "CONFIG_THINLTO=y" >> arch/arm64/configs/temp_defconfig

    make $BUILD_VAR temp_defconfig
    rm arch/arm64/configs/temp_defconfig

    make $BUILD_VAR

    # Handle DTB
    cat $(pwd)/out/arch/arm64/boot/dts/vendor/qcom/*.dtb > \
        $(pwd)/out/arch/arm64/boot/dts/vendor/qcom/dtb

    # Handle DTBO (fallback copy if already built)
    cp $(pwd)/out/arch/arm64/boot/dtbo.img dtbo.img 2>/dev/null || true
}

# -------------------------------
# DTB Build
# -------------------------------
build_dtb() {
    echo "-----------------------------------------------"
    echo "Building DTB..."
    echo "-----------------------------------------------"

    make $BUILD_VAR dtbs

    cat "$(pwd)/out/arch/arm64/boot/dts/vendor/qcom/kona.dtb" \
        "$(pwd)/out/arch/arm64/boot/dts/vendor/qcom/kona-v2.dtb" \
        "$(pwd)/out/arch/arm64/boot/dts/vendor/qcom/kona-v2.1.dtb" \
        > "$(pwd)/out/arch/arm64/boot/dts/dtb"
}

# -------------------------------
# DTBO Build
# -------------------------------
build_dtbo() {
    echo "-----------------------------------------------"
    echo "Building DTBO image..."
    echo "-----------------------------------------------"

    DTBO_FILES=$(find $(pwd)/out/arch/arm64/boot/dts/samsung/$DEVICE -name "kona-sec-$DEVICE-*.dtbo")
    $(pwd)/tools/mkdtimg create $(pwd)/out/dtbo.img --page_size=4096 ${DTBO_FILES}
}

# -------------------------------
# AnyKernel3 Packaging
# -------------------------------
package_zip() {
    echo "-----------------------------------------------"
    echo "📦 Packaging AnyKernel3 zip for $DEVICE"
    echo "-----------------------------------------------"

    cd AnyKernel3/ || exit 1

    # Copy kernel outputs
    mv "$(pwd)/../out/$DEVICE/arch/arm64/boot/Image.gz-dtb" Image.gz-dtb
    mv "$(pwd)/../out/$DEVICE/dtb" dtb
    [ -f "$(pwd)/../out/$DEVICE/dtbo.img" ] && mv "$(pwd)/../out/$DEVICE/dtbo.img" dtbo.img

    # Update device name in anykernel.sh
    sed -i "s/^device\.name1=.*/device.name1=${DEVICE}/" anykernel.sh

    # Create zip with timestamp
    BUILD_DATE=$(date +"%Y-%m-%d_%H-%M")
    ZIP_NAME="Bandido-Kernel-${DEVICE}-${BUILD_DATE}.zip"
    zip -r "../${ZIP_NAME}" *

    cd ..
    echo "✅ Build for $DEVICE completed at: $(date +"%A, %d %B %Y %H:%M")"
    echo "Output zip: $ZIP_NAME"
}

# -------------------------------
# Run All Steps
# -------------------------------
build_kernel
build_dtb
build_dtbo
package_zip
