#!/usr/bin/env python3
import imageio
import argparse

def create_rgb_dat(file):
    pic = imageio.imread(file)
    h, w, c = pic.shape
    out_name = file.split('.')[0]
    out_name = out_name + "{}x{}.dat".format(w,h)
    with open(out_name, 'w') as f:
        for row in range(h):
            for col in range(w):
                _r, _g, _b = pic[row,col]
                f.write("{} {} {}\n".format(_r, _g, _b))
    print("Done.")

def create_dat(file):
    pic = imageio.imread(file)
    h, w = pic.shape
    out_name = file.split('.')[0]
    out_name = out_name + "{}x{}.dat".format(w,h)
    with open(out_name, 'w') as f:
        for row in range(h):
            for col in range(w):
                f.write("{}\n".format(pic[row,col]))
    print("Done.")

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-r', '-rgb', action='store_true', default=False,
            help='Set input format RGB')
    ap.add_argument('-g', '-gray', action='store_true', default=False,
            help='Set input format greyscale')
    ap.add_argument('-i', '-input', type=str, required=True,
            help='Input image.')
    args = vars(ap.parse_args())
    print(args)

    if args['r']:
        create_rgb_dat(args['i'])
    if args['g']:
        create_dat(args['i'])

if __name__ == '__main__':
    main()
