import numpy as np
import tensorflow as tf
import cv2
import os

TFLITE_PATH = "User_model/cur_tflite/bearing_model.tflite"
NORMAL_DIR = "dataset/normal"
DEFECT_DIR = "dataset/defect"

# Load TFLite model
interpreter = tf.lite.Interpreter(model_path=TFLITE_PATH)
interpreter.allocate_tensors()
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

print("=== Model Info ===")
print(f"Input  shape: {input_details[0]['shape']}")
print(f"Input  dtype: {input_details[0]['dtype']}")
print(f"Input  quant: {input_details[0].get('quantization', 'N/A')}")
print(f"Output shape: {output_details[0]['shape']}")
print(f"Output dtype: {output_details[0]['dtype']}")
print(f"Output quant: {output_details[0].get('quantization', 'N/A')}")
print()

def run_one(path, label):
    img = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        print(f"  SKIP: cannot read {path}")
        return

    # Model expects int8 with quant params (scale=1.0, zero_point=-128)
    input_data = (img.astype(np.float32) - 128).astype(np.int8)

    for desc, data in tests.items():
        input_data = data.reshape(input_details[0]['shape'])
        interpreter.set_tensor(input_details[0]['index'], input_data)
        interpreter.invoke()
        output = interpreter.get_tensor(output_details[0]['index'])
        class_0, class_1 = output[0][0], output[0][1]
        verdict = 'DEFECT' if class_1 > class_0 else 'NORMAL'
        print(f"  [{label}] {desc:>18s} => class_0={class_0:>6d}  class_1={class_1:>6d}  => {verdict}")

# Test a few images
for label, dirpath in [("normal", NORMAL_DIR), ("defect", DEFECT_DIR)]:
    if not os.path.isdir(dirpath):
        print(f"Dir not found: {dirpath}")
        continue
    files = sorted(os.listdir(dirpath))[:5]
    print(f"--- {dirpath} ({len(os.listdir(dirpath))} imgs, testing {len(files)}) ---")
    for f in files:
        run_one(os.path.join(dirpath, f), label)
    print()
