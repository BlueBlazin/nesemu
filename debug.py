def debug():
    with open("nestest.log", 'r') as f:
        opcodes = [(line[:4], line[16:19]) for line in f]
    
    with open("emu.log", 'r') as f:
        logged = [line.strip().upper() for line in f]
    
    for i in range(len(logged)):
        if opcodes[i][0].rjust(4, '0') != logged[i].rjust(4, '0'):
            print(f"Failed on #{i}. "
                  f"Expected PC: {opcodes[i][0]}, logged: {logged[i]}. "
                  f"Bad opcode: {opcodes[i - 1][1]}")
            return

if __name__ == "__main__":
    debug()