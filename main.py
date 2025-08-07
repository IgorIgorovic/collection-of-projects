file = open("results.txt", "w")
nm = input("N M: \n").split(" ")
N = int(nm[0])
M = int(nm[1])
circuit = []

for x in range(N):
     row = []
     i = input()
     for j in i:
          if j == "\ ".strip():
               row.append("l")
          elif j == "/":
               row.append("r")
     circuit.append(row)


def solve(circuit, N, M):
     circuit_check  = circuit
     all_blocks = []
     currCheckPos = [[0, 0]]
     count = 0
     if connection_created(circuit_check, N, M):
          return "connection already created"
     for i in range(N):
          for j in range(M):
               all_blocks.append([i, j])
     while 1:
          for x in all_blocks:
               currCheckPos[len(currCheckPos) - 1] = x
               count += 1
               if count % len(all_blocks) == 1 and count > len(all_blocks):
                    if len(currCheckPos) < 2:
                         currCheckPos.insert(len(currCheckPos) - 1, all_blocks[0])
                    elif currCheckPos[len(currCheckPos) - 2] != all_blocks[len(all_blocks) - 1]:
                         currCheckPos[len(currCheckPos) - 2] = all_blocks[ all_blocks.index( currCheckPos[ len(currCheckPos) - 2 ] ) + 1]
                    else:
                         file.write("-1")
                         file.close()
                         return "this example needs more than 2 moves"

               for j in currCheckPos:
                    x = j[0]
                    y = j[1]
                    if circuit_check [x][y] == "l":
                         circuit_check [x][y] = "r"
                    elif circuit_check [x][y] == "r":
                         circuit_check [x][y] = "l"

               if connection_created(circuit, N, M):
                    file.write(str(len(currCheckPos)))
                    file.close()
                    return f"possible solution: {currCheckPos}"

               for j in currCheckPos:
                    x = j[0]
                    y = j[1]
                    if circuit_check [x][y] == "l":
                         circuit_check [x][y] = "r"
                    elif circuit_check [x][y] == "r":
                         circuit_check [x][y] = "l"


def connection_created(circuit, N, M):
     blocksToCheck = has_connection([0, 0], circuit)
     blocksToCheckNext = []
     checkedBlocks = []
     if circuit[0][0] == "r" or circuit[N-1][M-1] == "r":
          return False
     while blocksToCheck != []:
          for i in blocksToCheck:
               if not i in checkedBlocks:
                    if i == [N-1, M-1]:
                         return True
                    else:
                         checkedBlocks.append(i)
                         for x in has_connection(i, circuit):
                              blocksToCheckNext.append(x)
               else:
                    continue
          blocksToCheck = blocksToCheckNext.copy()
          blocksToCheckNext = []
     return False


def has_connection(this_block, circuit):
     result = []
     blocks_to_check1 = []
     blocks_to_check2 = []

     value = circuit[this_block[0]][this_block[1]]
     if value == "r":
          blocks_to_check1.append( [this_block[0] - 1,  this_block[1] + 1 ] )
          blocks_to_check1.append( [this_block[0] + 1, this_block[1] - 1 ] )

          blocks_to_check2.append( [this_block[0] -1,  this_block[1] ] )
          blocks_to_check2.append( [this_block[0], this_block[1] + 1] )
          blocks_to_check2.append( [this_block[0], this_block[1] - 1 ] )
          blocks_to_check2.append( [this_block[0] + 1, this_block[1]] )

     if value == "l":
          blocks_to_check1.append([this_block[0], this_block[1] - 1])
          blocks_to_check1.append([this_block[0], this_block[1] + 1])
          blocks_to_check1.append([this_block[0] + 1, this_block[1] ])

          blocks_to_check2.append([this_block[0] - 1, this_block[1] - 1 ])
          blocks_to_check2.append([this_block[0] + 1, this_block[1] + 1 ])

     for i in blocks_to_check1:
          x = i[0]
          y = i[1]
          if x >= 0 and y >= 0 and y <= M-1 and x <= N-1 and circuit[x][y] == "r":
               result.append(i)

     for i in blocks_to_check2:
          x = i[0]
          y = i[1]
          if x >= 0 and y >= 0 and y <= M-1 and x <= N-1 and circuit[x][y] == 'l':
               result.append(i)

     return result


print(solve(circuit, N, M))
