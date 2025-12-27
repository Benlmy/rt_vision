import cv2
import sys

# Usage: python plugin.py [resize/zoom] [inputFile] [outputFile] [param]

def process():
    if len(sys.argv) < 5:
        print("Error: Args missing")
        return

    op = sys.argv[1]
    input_path = sys.argv[2]
    output_path = sys.argv[3]
    param = float(sys.argv[4])

    img = cv2.imread(input_path)
    if img is None:
        print("Error: Read failed")
        return

    h, w = img.shape[:2]

   
    if op == "resize":
        
        new_w = int(param)
        new_h = int(h * (new_w / w))
        img = cv2.resize(img, (new_w, new_h))

    elif op == "zoom":
        
        scale = param
        if scale < 1.0: scale = 1.0
        
        
        center_x, center_y = w // 2, h // 2
        crop_w = int(w / scale)
        crop_h = int(h / scale)
        
        
        x1 = center_x - crop_w // 2
        x2 = center_x + crop_w // 2
        y1 = center_y - crop_h // 2
        y2 = center_y + crop_h // 2
        
        
        cropped = img[y1:y2, x1:x2]
        
        
        img = cv2.resize(cropped, (w, h))

    elif op == "rotate":
        if param == 90: img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)

    try:
        cv2.imwrite(output_path, img)
    except:
        pass

if __name__ == "__main__":
    process()