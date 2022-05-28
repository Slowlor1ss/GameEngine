from PIL import Image

f = open("output2.txt", "w")
im = Image.open('convert2Items.png')
imgData = im.getdata()

white = (255,255,255,255)
gray = (187, 187, 187, 255)
black = (0, 0, 0, 255)
lightBrown = (254, 181, 52, 255)
darkBrown = (217, 109, 36, 255)
yellow = (255, 255, 69, 255)
red = (254, 22, 29, 255)
green = (34, 254, 64, 255)
meatColor = (217, 109, 36, 255)

count = 0

# for i in imgData:
    # if (i == white):
        # f.write(" ")
    # elif (i == gray):
        # f.write("H")
    # elif (i == black):
        # f.write("_")
    # count += 1
    # if (count % im.size[0] == 0):
        # f.write("\n")

#Key to maps:
# 1 = top of bun
# 2 = lettuce
# 3 = meat
# 4 = bottom of bun
# 5 = cheese
# 6 = tomato
# _ = platform
# H = ladder
# P = where pepper is found
# X = player spawn

for i in imgData:
    if (i == white):
        f.write(" ")
    elif (i == gray):
        f.write("H")
    elif (i == black):
        f.write("_")
    elif (i == lightBrown):
        f.write("1")
    elif (i == green):
        f.write("2")
    elif (i == meatColor):
        f.write("3")
    elif (i == darkBrown):
        f.write("4")
    elif (i == yellow):
        f.write("5")
    elif (i == red):
        f.write("6")
    count += 1
    if (count % im.size[0] == 0):
        f.write("\n")

f.close()
print(im.size)
