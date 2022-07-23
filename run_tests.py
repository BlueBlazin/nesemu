from pathlib import Path
import os

if __name__ == "__main__":
    p = Path("blargg")
    # build emulator
    os.system("bazel build --cxxopt=-std=c++20 //src:main")

    for filepath in sorted(p.glob("*.nes"), key=lambda x: int(x.stem[:2])):
        save_path = f"screenshots/{filepath.stem}.png"
        # run test
        print(os.popen(f'./bazel-bin/src/main test "{filepath}" "{save_path}"').read())
