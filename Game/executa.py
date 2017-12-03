import os
import random
import time
import subprocess
from subprocess import Popen
from random import randint, shuffle
import re
import sys



def get_command(seed):
    c = "./Game -s "
    c += str(seed)
    c += " -i default.cnf "
    for i in range(0,4):
        c += PLAYERS[i] + " "
    c += "2>&1 > /dev/null | grep 'got score'"
    return c
  
  
random.seed(time.time())

PLAYERS = sys.argv[1:5]
punt = [0,0,0,0]
guany = [[0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]]
n = int(sys.argv[5])
count = 0

try:
  for i in range(0,n):
    
    com = get_command(random.randrange(10000))
    p = Popen(com, shell=True, stdout=subprocess.PIPE)
    used = [0,0,0,0]
    llista = [0,0,0,0]
    for j,line in enumerate(p.stdout.readlines()):
      llista[j] += int(re.findall("\d+", str(line))[-1])


    maxim = 0
    jugador = 1
    for j in range(0,4):
      punt[j] += llista[j]
      if maxim < llista[j]:
          jugador = j
          maxim = llista[j]
    guany[0][jugador] += 1
    used[jugador] = 1
    print("Ha guanyat " + PLAYERS[jugador])
    sys.stdout.flush()
    maxim = 0
    jugador = 1
    for j in range(0,4):
      if maxim < llista[j] and used[j] == 0:
          jugador = j
          maxim = llista[j]
    guany[1][jugador] += 1
    used[jugador] = 1

    maxim = 0
    jugador = 1
    for j in range(0,4):
      if maxim < llista[j] and used[j] == 0:
          jugador = j
          maxim = llista[j]
    guany[2][jugador] += 1
    used[jugador] = 1

    maxim = 0
    jugador = 1
    for j in range(0,4):
      if maxim < llista[j] and used[j] == 0:
          jugador = j
          maxim = llista[j]
    guany[3][jugador] += 1
    used[jugador] = 1
    count = count+1 
  
except KeyboardInterrupt:
    print()



print ("")

nom = ["", "", "", ""]
mitja = [0.0,0.0,0.0,0.0]
for i in range(0,4):
    nom[i] = sys.argv[i + 1]
    while len(nom[i]) < 12:
        nom[i] += str(" ")
    for j in range(0,4):
        mitja[i] += float(j + 1)*float(guany[j][i])/float(count)

for i in range(0,4):
  print("Jugador "+str(i + 1) + " " + nom[i] + ": " + str(punt[i]//count) + " " + str(guany[0][i]) + " "\
+ str(guany[1][i]) + " " + str(guany[2][i]) + " " + str(guany[3][i]) + " %.2f" % (mitja[i]))