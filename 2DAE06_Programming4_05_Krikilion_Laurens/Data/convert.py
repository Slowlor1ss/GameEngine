from PIL import Image

f = open("output.txt", "w")
im = Image.open('convert.png')
imgData = im.getdata()

white = (255,255,255,255)
gray = (187, 187, 187, 255)
black = (0, 0, 0, 255)

count = 0

for i in imgData:
    if (i == white):
        f.write(" ")
    elif (i == gray):
        f.write("H")
    elif (i == black):
        f.write("_")
    count += 1
    if (count % im.size[0] == 0):
        f.write("\n")

f.close()
print(im.size)
