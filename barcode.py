# import pyperclip
# import time

# oldSpam = pyperclip.paste()
# spam = oldSpam

# for i in range(108):
#     while spam == oldSpam:
#         spam = pyperclip.paste()
#         time.sleep(0.01)
#     oldSpam = spam

#     # print(f"{spam} > {int(spam, 2)} > 0x{int(spam, 2):03X}")
#     print(f"\t0x{int(spam, 2):03X}, // {chr(i+32)}")

import csv
with open('codes.csv') as csvfile:
    with open('src/codes.h', 'w') as codefile:
        codefile.write("// Code128 barcodes, generated from the Wikipedia table copied into codes.csv by barcode.py\n")
        codefile.write("// The row index is the ascii character-32\n")
        codefile.write("// The value uses the LSB 11 bits to encode the bars. 1=dark and the LSB is on the right\n")
        codefile.write("uint16_t barpatterns[] = {\n")
        spamreader = csv.reader(csvfile, delimiter=',', quotechar='"')
        i = 0
        for row in spamreader:
            # print(', '.join(row))
            try:
                print(f'\t0x{int(row[1], 2):03X}, // {row[0].strip()}')
                codefile.write(f'\t0x{int(row[1], 2):03X}, // {row[0].strip()}\n')
            except:
                print(i, row)
            i += 1
        codefile.write("};")