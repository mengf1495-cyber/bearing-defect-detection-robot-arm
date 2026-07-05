import tensorflow as tf

DATASET_DIR = r"E:\CIMC\AI\dataset"  # 改成你的数据集目录，里面应该有 normal 和 defect

ds = tf.keras.utils.image_dataset_from_directory(
    DATASET_DIR,
    image_size=(96, 96),
    color_mode="grayscale",
    batch_size=1,
    shuffle=False
)

print("class_names =", ds.class_names)