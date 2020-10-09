#!/usr/bin/env python3
def create_rgb_dat(file):\n",
    import imageio\n",
    pic = imageio.imread(file);\n",
    h, w, c = pic.shape\n",
    out_name = file.split('.')[0]\n",
    out_name = out_name + \"{}x{}.dat\".format(w,h)\n",
    with open(out_name, 'w') as f:\n",
        for row in range(h):\n",
            for col in range(w):\n",
                _r, _g, _b = pic[row,col]\n",
                f.write(\"{} {} {}\\n\".format(_r, _g, _b))\n",
    print(\"Done.\")"

def main():
    create_rgb_dat(screenshot.jpg)

if __name__ == '__main__':
    main()
