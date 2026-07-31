import sys
import os

try:
    from PIL import Image
except ImportError:
    print("Vui lòng cài đặt thư viện Pillow: pip install pillow")
    sys.exit(1)

if len(sys.argv) < 3:
    print("Sử dụng: python convert_gif.py <input.gif> <output.h>")
    sys.exit(1)

input_path = sys.argv[1]
output_path = sys.argv[2]

try:
    img = Image.open(input_path)
except Exception as e:
    print(f"Lỗi khi mở file: {e}")
    sys.exit(1)

frames = []
try:
    while True:
        # Xử lý transparency
        if img.mode in ('RGBA', 'LA') or (img.mode == 'P' and 'transparency' in img.info):
            frame = img.convert('RGBA')
            # Nền đen
            background = Image.new("RGBA", frame.size, (0, 0, 0, 255))
            background.paste(frame, mask=frame.split()[3])
        else:
            background = img.convert('RGB')
            
        # Resize về 128x64
        resized = background.resize((128, 64), Image.Resampling.LANCZOS)
        
        # Chuyển sang trắng đen (1-bit) với Floyd-Steinberg dithering hoặc threshold
        bw = resized.convert("1")
        
        pixels = list(bw.getdata())
        width, height = bw.size
        
        byte_array = []
        for y in range(height):
            for x_byte in range(0, width, 8):
                byte_val = 0
                for bit in range(8):
                    if x_byte + bit < width:
                        pixel = pixels[y * width + x_byte + bit]
                        # Trong ảnh 1-bit của PIL, 255 là trắng, 0 là đen.
                        # Adafruit GFX drawBitmap thường coi bit 1 là vẽ màu, bit 0 là bỏ qua.
                        if pixel > 127:
                            byte_val |= (1 << (7 - bit)) # MSB first
                byte_array.append(byte_val)
        
        frames.append(byte_array)
        img.seek(img.tell() + 1)
except EOFError:
    pass # Hết frame

# Tạo thư mục chứa output nếu chưa có
os.makedirs(os.path.dirname(output_path), exist_ok=True)

with open(output_path, "w") as f:
    f.write("#ifndef GIF_FRAMES_H\n")
    f.write("#define GIF_FRAMES_H\n\n")
    f.write("#include <Arduino.h>\n")
    f.write("#include <pgmspace.h>\n\n")
    
    f.write(f"const int gif_frame_count = {len(frames)};\n\n")
    
    for i, frame in enumerate(frames):
        f.write(f"const unsigned char gif_frame_{i}[] PROGMEM = {{\n")
        lines = []
        for j in range(0, len(frame), 16):
            chunk = frame[j:j+16]
            lines.append("  " + ", ".join([f"0x{b:02X}" for b in chunk]))
        f.write(",\n".join(lines))
        f.write("\n};\n\n")
        
    f.write("const unsigned char* const gif_frames[] PROGMEM = {\n")
    f.write("  " + ", ".join([f"gif_frame_{i}" for i in range(len(frames))]))
    f.write("\n};\n\n")
    
    f.write("#endif\n")

print(f"Thành công! Đã chuyển {len(frames)} frames thành file C++ header tại {output_path}")
