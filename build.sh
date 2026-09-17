#! /bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"

if ! docker image inspect devkitpro/devkita64:latest >/dev/null 2>&1; then
    echo "Pulling devkitPro switch toolchain image..."
    docker pull devkitpro/devkita64:latest
fi

docker run --rm -u "$(id -u):$(id -g)" \
    -v "$PROJECT_DIR:/workspace" \
    -w /workspace \
    devkitpro/devkita64:latest bash -c '
        source /opt/devkitpro/switchvars.sh 2>/dev/null
        export DEVKITPRO=/opt/devkitpro
        export DEVKITA64=/opt/devkitpro/devkitA64
        export PORTLIBS_PREFIX=/opt/devkitpro/portlibs/switch
        export PATH=/opt/devkitpro/tools/bin:/opt/devkitpro/devkitA64/bin:$PATH
        cmake -B build_switch -DPLATFORM_SWITCH=ON -DCMAKE_BUILD_TYPE=Release
        make -C build_switch Simple_Mod_Alchemist.nro -j$(nproc)
    '

echo
echo "Build successful: $PROJECT_DIR/build_switch/Simple_Mod_Alchemist.nro"
