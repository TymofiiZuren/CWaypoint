"""Stage the explicitly approved pinned tools in a local app bundle."""
from pathlib import Path
import hashlib
import json
import shutil
import struct
import subprocess
import sys
root, bundle = map(Path, sys.argv[1:3])
sdk = root / '.scratch/approved-tools/wasi-sdk-34.0-arm64-macos'
runtime = root / '.scratch/approved-tools/wasmtime-v50.0.1-aarch64-macos'
target = bundle / 'Contents/Resources/LabTools'
if not sdk.exists() or not runtime.exists():
    sys.exit('Approved lab tools missing. Run scripts/setup_lab_tools.py first.')
marker = target / 'tools-version.txt'
version = 'wasi-sdk-34;wasmtime-50.0.1;package-1'
target.mkdir(parents=True, exist_ok=True)
shutil.copytree(root / 'resources/licenses', target / 'licenses', dirs_exist_ok=True)
if marker.exists() and marker.read_text() == version:
    sys.exit(0)
target.mkdir(parents=True, exist_ok=True)
shutil.copytree(sdk, target / 'wasi-sdk', dirs_exist_ok=True, symlinks=True)
shutil.copy2(runtime / 'wasmtime', target / 'wasmtime')
shutil.copy2(runtime / 'LICENSE', target / 'WASMTIME-LICENSE')
# Sign nested native executables for sandbox inheritance; Wasm/archive data is not executable Mach-O.
for path in target.rglob('*'):
    if path.is_symlink() or not path.is_file():
        continue
    with path.open('rb') as stream:
        header = stream.read(16)
    if len(header) < 16 or header[:4] not in (b'\xcf\xfa\xed\xfe', b'\xce\xfa\xed\xfe'):
        continue
    args = ['codesign', '--force', '--sign', '-']
    if struct.unpack('<I', header[12:16])[0] == 2:
        args += ['--entitlements', str(root / 'resources/LabChild.entitlements')]
    subprocess.run(args + [str(path)], check=True, capture_output=True)
marker.write_text(version)
