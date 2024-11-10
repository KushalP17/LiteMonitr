import os, sys
from PIL import Image

path = os.getcwd()

f = open(path + "//header.h", "w")

for File in os.listdir(path):
    try:
        im = Image.open(path + "//" + File, 'r')
        pix = im.load()
        f.write("//" + File + " " + str(im.height) + "x" + str(im.width) + "px\n")
        f.write("const uint8_t " + File[0:-4] + "[" + str(im.height) + "][" + str(im.width) + "][" + str(3) + "]= {\n")
        idx = 0
        for hidx in range(im.height):
            for widx in range(im.width):
                if idx == 0:
                    f.write(str(pix[widx, hidx][0]) + ", " + str(pix[widx, hidx][1]) + ", " + str(pix[widx, hidx][2]))
                else:
                    f.write(", " + str(pix[widx, hidx][0]) + ", " + str(pix[widx, hidx][1]) + ", " + str(pix[widx, hidx][2]))
                # print(pix[widx, hidx])
                idx += 1

        f.write("\n};\n\n")
    except OSError:
        pass
