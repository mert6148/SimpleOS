#!/usr/bin/env sh
set -eu

here="$(CDPATH='' cd "$(dirname "$0")" && pwd)"
cd "$here"

usage() {
    echo "Usage: $0 [x64|x86]"
    exit 1
}

arch_arg="${1:-x64}"
case "$arch_arg" in
    x64|x86_64)
        TARGET_ARCH=x64
        ;;
    x86|i386|i686)
        TARGET_ARCH=x86
        ;;
    *)
        echo "Error: Unsupported architecture '$arch_arg'"
        usage
        ;;
esac

if [ "$(id -u)" -ne 0 ]; then
    echo "Please run this installer with sudo or as root."
    exit 1
fi

echo "[SimpleOS] Installing Linux runtime dependencies for $TARGET_ARCH..."
apt-get update
apt-get install -y gcc g++ make python3 python3-pip php apache2 dotnet-sdk-6.0
if [ "$TARGET_ARCH" = "x86" ]; then
    apt-get install -y gcc-multilib g++-multilib libc6-dev-i386
fi
pip3 install flask flask-cors psutil

echo "[SimpleOS] Building SimpleOS for $TARGET_ARCH..."
make ARCH="$TARGET_ARCH" install

install_root="/opt/simpleos/$TARGET_ARCH"
run_script="$install_root/run.sh"
bin_dir="$install_root/bin"

echo "[SimpleOS] Creating install root at $install_root..."
mkdir -p "$bin_dir"
mkdir -p "$install_root/services"
cp -r services/php "$install_root/services/" 2>/dev/null || true
cp -r services/csharp "$install_root/services/" 2>/dev/null || true
cp .env.example "$install_root/" 2>/dev/null || true
cp "build/linux/$TARGET_ARCH/kernel" "$bin_dir/kernel"

cat > "$run_script" <<'EOF'
#!/usr/bin/env sh
set -eu
BASE="$(CDPATH='' cd "$(dirname "$0")" && pwd)"
KERNEL_BIN="$BASE/bin/kernel"

cd "$BASE"
exec "$KERNEL_BIN"
EOF
chmod +x "$run_script"

service_unit="/etc/systemd/system/simpleos-$TARGET_ARCH.service"
if command -v systemctl >/dev/null 2>&1; then
    echo "[SimpleOS] Creating systemd unit $service_unit..."
    cat > "$service_unit" <<SERVICE_EOF
[Unit]
Description=SimpleOS runtime ($TARGET_ARCH)
After=network.target

[Service]
Type=simple
WorkingDirectory=$install_root
ExecStart=$run_script
Restart=on-failure
User=root
Environment=PATH=/usr/bin:/bin

[Install]
WantedBy=multi-user.target
SERVICE_EOF
    systemctl daemon-reload
    systemctl enable "simpleos-$TARGET_ARCH.service"
    echo "[SimpleOS] Enabled simpleos-$TARGET_ARCH.service. Start it with: sudo systemctl start simpleos-$TARGET_ARCH.service"
else
    echo "[SimpleOS] systemctl not available; skipping service installation."
fi

cat <<EOF
[SimpleOS] Installation complete.
  Architecture: $TARGET_ARCH
  Installed to: $install_root

To start the Linux runtime manually:
  sudo sh "$run_script"

To build again for the same architecture:
  make ARCH=$TARGET_ARCH build
EOF
