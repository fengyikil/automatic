from PIL import Image

# 打开PNG文件
png_image = Image.open("./yao.png")

# 保存为ICO文件
png_image.save("output_icon.ico", format="ICO")