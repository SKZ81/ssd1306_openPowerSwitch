#!/usr/bin/env python3

import sys

def usage():
    print(sys.argv[0] + 'inuput.xbm output.c')
    print('Will convert a classical XBM image file to a C (pgmspace) array suitable to use with SSD1306')
    sys.exit(1)



def xbm2ssd1306(input_filename, output_filename, debug=False):
    width = -1
    height = -1
    bits = []
    reading_bits = False
    start_bits_found = False
    end_bits_found = False

    fin = open(input_filename, 'r')
    for line in fin:
        if line.startswith('#define _width '):
            width = int( line[len('#define _width '):-1] )
            print('width :', str(width))
        if line.startswith('#define _height '):
            height = int( line[len('#define _height '):-1] )
            print('height :', str(height))
        if "{" in line:
            line = line[line.find('{')+1:-1]
            start_bits_found = True
            reading_bits = True
        if '}' in line:
            end_bits_found = True
            line = line[0:line.find('}')]
        if reading_bits:
            bytes = [byte.strip() for byte in line.split(',')]
            for byte in bytes:
                if byte.startswith('0x'):
                    value = int(byte[2:], 16)
                    bits.append(value)
                    if debug:
                        sys.stdout.write(str(value)+', ')
                else:
                    if byte != '' and debug:
                        #we found something strange, warn...
                        print()
                        print('found non-numerical value: '+byte+ ' ignored...')
        if end_bits_found:
            if debug:
                print()
            reading_bits = False

    fin.close()

    # make some checks
    error = False
    if not start_bits_found:
        print('missing opening brace {')
        error = True
    if not end_bits_found:
        print('missing closing brace }')
        error = True
    if width == -1 or height == -1:
        print('missing image dimension')
        error = True
    elif len(bits) != width*height/8:
        print('Number of value mismatch dimensions (found: ' + str(len(bits)) + ', expected: ' + str(width*height/8))
        error = True
    if error:
        return 2

    #TODO !! what if width % 8 !=0 ?? check XBM format in this case first
    def get_bit(w, h):
        if w>=width or h>=height:
            return 0
        else:
            return (bits[h*-(-width//8) + (w//8)] & (1 << (w%8))) >> (w%8)

    w,h = 0,0
    bits_out=[]
    while h < height:
        while w < width:
            out_byte = get_bit(w, h) | get_bit(w, h+1) << 1 | get_bit(w, h+2) << 2 | get_bit(w, h+3) << 3 | get_bit(w, h+4) << 4 | get_bit(w, h+5) << 5 | get_bit(w, h+6) << 6 | get_bit(w, h+7) << 7

            bits_out.append(out_byte)
            w += 1
        w=0
        h += 8

    if debug:
        w,h = 0,0
        while h < height:
            while w < width:
                if get_bit(w, h):
                    sys.stdout.write('*')
                else:
                    sys.stdout.write(' ')
                w+=1
            w=0
            sys.stdout.write('|\n')
            h+=1

    fout = open(output_filename, 'w')
    fout.write('#define _width ' + str(width) + '\n');
    fout.write('#define _height ' + str(height) + '\n');
    fout.write('static const unsigned char _bits[] PROGMEM = {\n');
    count = 0
    for byte in bits_out[:-1]:
        fout.write('0x%0.2X' % byte)
        fout.write(',')
        count += 1
        if count == 16:
            fout.write('\n')
            count = 0

    fout.write('0x%0.2X\n};' % bits_out[-1])

    fout.close()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        usage()

    sys.exit( xbm2ssd1306(sys.argv[1], sys.argv[2], debug=True))
