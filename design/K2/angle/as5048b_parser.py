# -*- coding: utf-8 -*-
"""
Created on Mon Oct 28 15:19:30 2019

@author: xkadj
"""

#import csv
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

#path = r"C:\Users\xkadj\OneDrive\PROJEKTY\Projekt_ROBOTIKA\K2\angle\arduino_log_as5108b_1mm.txt"
#path = r"C:\Users\xkadj\OneDrive\PROJEKTY\Projekt_ROBOTIKA\K2\angle\arduino_log_as5108b_3mm.txt"
#path = r"C:\Users\xkadj\OneDrive\PROJEKTY\Projekt_ROBOTIKA\K2\angle\arduino_log_as5108b_8mm.txt"
#path = r"C:\Users\xkadj\OneDrive\PROJEKTY\Projekt_ROBOTIKA\K2\angle\arduino_log_as5108b_3mm_sci.txt"
path = r"C:\Users\xkadj\OneDrive\PROJEKTY\Projekt_ROBOTIKA\K2\ardiono_modules\arduino_log.txt"

# =============================================================================
# 
# =============================================================================
def parser(path):    
    rows = pd.read_csv(path, sep=',', engine='python').values.tolist()
    for row in range(len(rows)):
        if rows[row] == ['Serial OK']: start_of_header = row
        if rows[row] == ['Found 1 device(s).']: end_of_header = row    
            
    header, data = rows[start_of_header:end_of_header], rows[end_of_header+1:]
    
    binary, dec = [],[]
    for row in range(len(data)):
        if len(str(data[row]).split(';')) == 2: binary.append(str(data[row]).split(';'))
        if len(str(data[row]).split(';')) == 1: dec.append(str(data[row]))
    
    dec = dec_str_list_to_np_int(dec)
    binary = bin_str_list_to_np_int(binary)
    
    return binary, dec, header

def dec_str_list_to_np_int(str_list):
    int_l = np.array([0])
    for string in range(len(str_list)):
        value = int(str_list[string].split("'")[1])
        int_l = np.vstack((int_l,value))
    return int_l[1:]

def bin_str_list_to_np_int(str_list):
    int_l = np.array([0,0,0])
    for string in range(len(str_list)):
        value_255 = int(str_list[string][0].split("'")[1],2)
        value_254 = int(str_list[string][1].split("'")[0],2)
        value = value_255 * 256 + value_254
        int_l = np.vstack((int_l,[value_255,value_254,value]))
    return int_l[1:]

# =============================================================================
# 
# =============================================================================

binary, dec, header = parser(path)

plt.plot(binary.T[2],'x-')
plt.xlabel('sample[-]')
plt.ylabel('angle[-]')
plt.show()


