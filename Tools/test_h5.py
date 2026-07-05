import numpy as np
import tensorflow as tf
import cv2
import os
import sys

if len(sys.argv) < 4:
    print("Usage: python test_h5.py <model.h5> <dataset/normal/sample.png> <dataset/defect/sample.png>")
    sys.exit(1)

model_path = sys.argv[1]
print(f"Loading {model_path} ...")
model = tf.keras.models.load_model(model_path)
print(f"Input shape: {model.input_shape}")
print(f"Output shape: {model.output_shape}")

for path in sys.argv[2:]:
    img = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
    label = "normal" if "normal" in path else "defect"

    # try int8 input
    input_data = (img.astype(np.float32) - 128).astype(np.int8).reshape(1, 96, 96, 1)
    out = model.predict(input_data, verbose=0)
    print(f"[{label}] int8  => {out[0]} => {'DEFECT' if out[0][1] > out[0][0] else 'NORMAL'}")

    # try float input
    input_data_f = (img.astype(np.float32) / 255.0).reshape(1, 96, 96, 1)
    out_f = model.predict(input_data_f, verbose=0)
    print(f"[{label}] float => {out_f[0]} => {'DEFECT' if out_f[0][1] > out_f[0][0] else 'NORMAL'}")
