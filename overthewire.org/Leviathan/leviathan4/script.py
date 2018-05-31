#!/usr/bin/python

bin_str = '01010100 01101001 01110100 01101000 00110100 01100011 01101111 01101011 01100101 01101001 00001010'

bin_list = bin_str.split()
char_list = []

i = 0


while i < len(bin_list) :
	char_list.append(chr(int(bin_list[i], 2)))
	i = i + 1


final_string = ''.join(str(char) for char in char_list)

print final_string
