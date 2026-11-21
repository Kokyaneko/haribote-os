# makefont.py
# フォントファイルをコンパイルするために使用する.
import sys


def main():
    if len(sys.argv) < 3:
        print("Usage: python3 makefont.py <input_txt> <output_c>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    font_data = []

    with open(input_file, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            line = line.rstrip("\r\n")
            # 8文字かつ '*' と '.' だけで構成される行をフォントの1ラインとみなす
            if len(line) == 8 and all(c in ("*", ".") for c in line):
                byte_val = 0
                for c in line:
                    byte_val = (byte_val << 1) | (1 if c == "*" else 0)
                font_data.append(byte_val)

    # 256文字 × 16行 = 4096バイトになっているかチェック
    if len(font_data) != 4096:
        print(f"Warning: Expected 4096 bytes, but got {len(font_data)} bytes.")

    with open(output_file, "w", encoding="utf-8") as f:
        f.write("char hankaku[4096] = {\n")
        for i in range(0, len(font_data), 16):
            chunk = font_data[i : i + 16]
            hex_str = ", ".join(f"0x{b:02x}" for b in chunk)
            comma = "," if i + 16 < len(font_data) else ""
            f.write(f"    {hex_str}{comma}\n")
        f.write("};\n")

    print(f"Successfully generated {output_file} ({len(font_data)} bytes)")


if __name__ == "__main__":
    main()
