import re

keywords = {
    "ADD":   "0001", 
    "AND":   "0101",
    "NOT":   "1001",
    "LD" :   "0010",
    "LDR":   "0110",
    "LDI":   "1010",
    "LEA":   "1110",
    "ST" :   "0011",
    "STR":   "0111",
    "STI":   "1011",
    "RTI":   "",
    # TRAPs
    "TRAP":  "",
    "GETC":  "x20",
    "OUT" :  "x21",
    "PUTS":  "x22",
    "IN"  :  "x23",
    "PUTSP": "x24",
    "HALT":  "x25",
    # BRs
    "BR" :   "0000111",
    "BRn":   "0000100",
    "BRz":   "0000010",
    "BRp":   "0000001",
    "BRnz":  "0000110",
    "BRzp":  "0000011",
    "BRnp":  "0000101",
    "BRnzp": "0000111",
    # JMPs
    "JMP":   "",
    "RET":   "",
    # JSRs
    "JSR":   "",
    "JSRR":  "",
    # Psedo Ops
    ".ORIG": "",
    ".FILL": "",
    ".BLKW": "0111011101110111",
    ".STRINGZ": ""
}

regs = {
    "R0": "000",
    "R1": "001",
    "R2": "010",
    "R3": "011",
    "R4": "100",
    "R5": "101",
    "R6": "110",
    "R7": "111"
}

symbols = {}

def binN(immStr, base, len):
    mask = {
        5 : 0x1F,
        6 : 0x3F,
        8 : 0xFF,
        9 : 0x1FF,
        11: 0x7FF,
        16: 0xFFFF
    }
    if immStr[1] == '-':
        if int(immStr[1:], base) == 0:
            return str("").rjust(len, '0')
        else:
            return str(bin(int(immStr[1:], base) & mask[len]))[2:]
    else:
        return str(bin(int(immStr[1:], base)))[2:].rjust(len, '0')

def str2binN(immStr, len):
    if immStr[0] == 'x':
        return binN(immStr, 16, len)
    else:
        return binN(immStr, 10, len)
    
def label2binN(pc, label, len):
    offset = "#" + str(symbols[label] - pc)
    return str2binN(offset, len)

lineNo  = -1
todo    = []
# main loop
while True:
    line = input()
    curline = re.split(',|\s', line)
    curline = list(filter(lambda x : len(x)>0, curline)) 

    # empty line & end
    if len(curline) == 0:
        continue
    if curline[0] == ".END":
        break

    lineNo  += 1

    # if(label)
    token = curline[0]
    if token not in keywords.keys():
        symbols[token] = lineNo
        del curline[0]
        token = curline[0]
    
    if token == ".STRINGZ":
        begin = line.find('\"')
        end   = line.find('\"', begin+1)
        curline = [".STRINGZ", line[begin+1:end]]
    
    # trim()
    curline.insert(0, lineNo)
    todo.append(curline)

    # lineNo handle: .BLKW / .STRINGZ
    if   token == ".BLKW":
        lineNo += int(curline[2][1:]) - 1
    elif token == ".STRINGZ":
        lineNo += len(curline[2])

for line in todo:
    pc = line[0]+1
    # handle
    token = line[1]
    # pseudo ops
    if   token == ".ORIG":
        res = str2binN(line[2], 16)
        print(res)
    elif token == ".FILL":
        res = str2binN(line[2], 16)
        print(res)
    elif token == ".BLKW":
        for i in range(int(line[2][1:])):
            res = keywords[token]
            print(res)
    elif token == ".STRINGZ":
        for ch in line[2]:
            res = str(bin(ord(ch)))[2:].rjust(16, '0')
            print(res)
        res = "0000000000000000"
        print(res)
    # calculations
    elif token in ["ADD", "AND"]:
        res = keywords[token] + regs[line[2]] + regs[line[3]]
        # is arg3 reg/imm5 ?
        if line[4][0] == 'R':
            res += "000" + regs[line[4]]
        else:
            res += "1" + str2binN(line[4], 5)
        print(res)
    elif token == "NOT":
        res = keywords[token] + regs[line[2]] + regs[line[3]] + "111111"
        print(res)
    elif token in ["LD", "ST", "LDI", "STI", "LEA"]:
        res = keywords[token] + regs[line[2]]
        if line[3][0] == '#':
            res += str2binN(line[3], 9)
        else:
            res += label2binN(pc, line[3], 9)
        print(res)
    elif token in ["LDR", "STR"]:
        res = keywords[token] + regs[line[2]] + regs[line[3]] + str2binN(line[4], 6)
        print(res)
    # traps
    elif token in ["TRAP", "GETC", "OUT", "PUTC", "PUTS", "IN", "PUTSP", "HALT"]:
        res = "11110000"
        # vector or instruction?
        if token == "TRAP":
            res += str2binN(line[2], 8)
        else:
            res += str2binN(keywords[token], 8)
        print(res)
    # branched
    elif token in ["BR", "BRn", "BRz", "BRp", "BRnz", "BRzp", "BRnp", "BRnzp"]:
        res = keywords[token]
        if line[2][0] == '#':
            res += str2binN(line[2], 9)
        else:
            res += label2binN(pc, line[2], 9)
        print(res)
    # JMPs
    elif token == "JMP":
        res = "1100000" + regs[line[2]] +"000000"
        print(res)
    elif token == "RET":
        res = "1100000111000000"
        print(res)
    # JSRs
    elif token == "JSR":
        res = "01001"
        if line[2][0] == '#':
            res += str2binN(line[2], 11)
        else:
            res += label2binN(pc, line[2], 11)
        print(res)
    elif token == "JSRR":
        res = "0100000" + regs[line[2]] + "000000"
        print(res)
    elif token == "RTI":
        res = "1000000000000000"
        print(res)