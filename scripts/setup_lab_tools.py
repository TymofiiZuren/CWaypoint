"""Download only the two approved, pinned Apple Silicon lab-tool releases."""
import hashlib
import tarfile
import urllib.request
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1] / '.scratch/approved-tools'
ASSETS = [
 ('WebAssembly/wasi-sdk','wasi-sdk-34','wasi-sdk-34.0-arm64-macos.tar.gz','9c59398106b417f8f14913380fdf0097a8cc0ff4af9eb3ce0065a859e88d49e9'),
 ('bytecodealliance/wasmtime','v50.0.1','wasmtime-v50.0.1-aarch64-macos.tar.xz','960e5216c7ebd7e9b45e6561d15821b8b801b7d54aee28c7b91921aeb30c65c8')]
ROOT.mkdir(parents=True, exist_ok=True)
for repo, version, name, digest in ASSETS:
    archive = ROOT / name
    if not archive.exists():
        urllib.request.urlretrieve(f'https://github.com/{repo}/releases/download/{version}/{name}', archive)
    if hashlib.sha256(archive.read_bytes()).hexdigest() != digest:
        raise SystemExit(f'Checksum mismatch: {name}; no extraction performed.')
    with tarfile.open(archive) as tar:
        tar.extractall(ROOT, filter='data')
    print(f'Verified and extracted {name}')
