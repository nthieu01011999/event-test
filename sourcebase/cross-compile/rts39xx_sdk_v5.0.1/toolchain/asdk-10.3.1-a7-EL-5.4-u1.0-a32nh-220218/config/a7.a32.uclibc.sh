DIR_WRAPPER="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd )"
ASAN_OFFSET=""
BUILD_SECURE="false"
BUILD_VERBOSE="false"
COMPILE_OPTION=" -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=auto "
CONFIG_NAME="a7.a32.uclibc"
LIBC_NAME="uclibc"
LIBC_VERSION="1.0"
MULTILIB_LIST="(-mfloat-abi=hard -mfloat-abi=softfp)"
SDK="asdk"
TARGET_AS="arm-linux-uclibcgnueabi-gcc"
TARGET_CC="arm-linux-uclibcgnueabi-gcc"
TARGET_CXX="arm-linux-uclibcgnueabi-g++"
TARGET_ENDIAN="EL"
TARGET_HAS_FPU="true"
TARGET_NAME="arm-linux-uclibcgnueabi"
TARGET_OPTION=" -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=auto"
