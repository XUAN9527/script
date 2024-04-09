import os
import shutil
import struct
import binascii

bin1_path = 'bootloader.bin'
bin2_path = 'app.bin'
bin_result_path = 'mix.bin'
hex_result_path = 'mix.hex'

offset1 = 0x00000000
offset2 = 0x00008000

shutil.copyfile(bin1_path, bin_result_path)

bin_1 = open(bin1_path, 'rb')
bin_2 = open(bin2_path, 'rb')
bin_merge = open(bin_result_path, 'ab')
bin2_size = os.path.getsize(bin2_path)

bin_result_size = os.path.getsize(bin_result_path)
final_size = offset2 + bin2_size

offset = bin_result_size
value_default = struct.pack('B', 0xff)

while offset < final_size:
    if offset == offset2:
        data = bin_2.read()
        bin_merge.write(data)
        offset = bin_merge.tell()
    else:
        bin_merge.write(value_default)
        offset = bin_merge.tell()

bin_1.close()
bin_2.close()
bin_merge.close()
